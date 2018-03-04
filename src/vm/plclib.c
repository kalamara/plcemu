#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "config.h"
#include "../hw/hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "util.h"

#include "project.h"

const char * LibErrors[N_IE] = {
    "Unknown error",
    "Invalid operator",
    "Invalid output",
    "Invalid numeric index",
    "Invalid operand",
    "File does not exist",
    "Unreadable character"
};

struct timeval Curtime;

int open_pipe(const char * pipe, plc_t p) {
    p->com[0].fd = open(pipe, O_NONBLOCK | O_RDONLY);
    p->com[0].events = POLLIN | POLLPRI;
    int r = p->com[0].fd > 0? PLC_OK: PLC_ERR;

    gettimeofday(&Curtime, NULL);
    
    return r;
}

int re(const plc_t p,  int type,  int idx) { 
//return rising edge of operand

	switch (type){
    case BOOL_DI:
		return (p->di[idx].RE);
		break;
    case BOOL_COUNTER:
		return (p->m[idx].PULSE) && (p->m[idx].EDGE);
		break;
	default:
        return PLC_ERR;
	}
}

int fe(const plc_t p,  int type,  int idx) { 
//return falling edge of operand

	switch (type) {
    case BOOL_DI:
		return (p->di[idx].FE);
		break;
    case BOOL_COUNTER:
		return (!p->m[idx].PULSE) && (p->m[idx].EDGE);
		break;
	default:
        return PLC_ERR;
	}
}

int set(plc_t p,  int type,  int idx) { 
//set operand
	switch (type){
    case BOOL_DQ:
        if(idx / BYTESIZE >= p->nq)
            return ERR_BADOPERAND;
		p->dq[idx].SET = TRUE;
		p->dq[idx].RESET = FALSE;
		break;
    case BOOL_COUNTER:
        if(idx >= p->nm)
            return ERR_BADOPERAND;
		p->m[idx].SET = TRUE;
		p->m[idx].RESET = FALSE;
		if (!p->m[idx].PULSE)
			p->m[idx].EDGE = TRUE;
		break;
    case BOOL_TIMER:
        if(idx >= p->nt)
            return ERR_BADOPERAND;
		p->t[idx].START = TRUE;
		break;
	default:
        return PLC_ERR;
	}
    return 0;
}

int reset(plc_t p,  int type, int idx) { 
//reset operand
	switch (type){
    case BOOL_DQ:
        if(idx / BYTESIZE >= p->nq)
            return ERR_BADOPERAND;
            
		p->dq[idx].RESET = TRUE;
		p->dq[idx].SET = FALSE;
		break;
    case BOOL_COUNTER:
        if(idx >= p->nm)
            return ERR_BADOPERAND;
            
		p->m[idx].RESET = TRUE;
		p->m[idx].SET = FALSE;
		if (p->m[idx].PULSE)
			p->m[idx].EDGE = TRUE;
		break;
    case BOOL_TIMER:
        if(idx >= p->nt)
            return ERR_BADOPERAND;
            
		p->t[idx].START = FALSE;
		break;
	default:
        return PLC_ERR;
	}
    return 0;
}

int contact(plc_t p, 
             int type, 
             int idx, 
             BYTE val) { 
//contacts an output with a value
	switch (type){
    case BOOL_DQ:
        if(idx / BYTESIZE >= p->nq)
            return ERR_BADOPERAND;
            
		p->dq[idx].Q = val;
		break;
    case BOOL_COUNTER:
        if(idx >= p->nm)
            return ERR_BADOPERAND;
            
		if (p->m[idx].PULSE != val)
			p->m[idx].EDGE = TRUE;
		else
			p->m[idx].EDGE = FALSE;
		p->m[idx].PULSE = val;
		break;
    case BOOL_TIMER:
        if(idx >= p->nt)
            return ERR_BADOPERAND;
            
		p->t[idx].START = TRUE;
		break;
	default:
        return PLC_ERR;
	}
    return 0;
}

int resolve(plc_t p,  int type,  int idx) { 
//return an operand value
	switch (type){
    case BOOL_DQ:
        return p->dq[idx].Q 
           || (p->dq[idx].SET && !p->dq[idx].RESET);
           
    case BOOL_COUNTER:
		return p->m[idx].PULSE;
		
    case BOOL_DI:
		return p->di[idx].I;
		
    case BOOL_BLINKER:
		return p->s[idx].Q;
		
    case BOOL_TIMER:
		return p->t[idx].Q;
		
	default:
        return PLC_ERR;
	}
}

int down_timer(plc_t p,  int idx) { 
//RESET timer
	p->t[idx].START = FALSE;
	p->t[idx].V = 0;
	p->t[idx].Q = 0;
    return 0;
}

int task(long timeout, plc_t p, rung_t r) {
    unsigned int i = 0;
    unsigned int pc = 0;
    struct timeval start;
    struct timeval lapse;
    
    gettimeofday(&start,NULL);

    long delta = 0;
    
    if(r==NULL
    || p==NULL)
        return PLC_ERR;
     
    int rv = 0;    
	while(rv >= PLC_OK && i < r->insno){
	    if(delta >= timeout){
	        rv = ERR_TIMEOUT;
	        break;
	    }    
	    pc = i;
		rv = instruct(p, r, &pc);
		if (rv < PLC_OK){
		    switch(rv){
                case PLC_ERR:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_PLC]);
                    break;
                case ERR_BADOPERATOR:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADOPERATOR]);
                    break;
                case ERR_BADCOIL:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADCOIL]);
                    break;
                case ERR_BADINDEX:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADINDEX]);
                    break;
                case ERR_BADOPERAND:
                    plc_log("Instruction %d :%s", i, 
                            LibErrors[IE_BADOPERAND]);
                    break;
                case ERR_BADFILE:
                    plc_log("Instruction %d :%s", i, 
                             LibErrors[IE_BADFILE]);
                    break;
                case ERR_BADCHAR:
                    plc_log("Instruction %d :%s", i, 
                             LibErrors[IE_BADCHAR]);
                    break;    
                default: break;
            }
		}
	    gettimeofday(&lapse,NULL);
        delta = lapse.tv_usec - start.tv_usec;
		//plc_log("Instruction %d : OK", i); 
		i = pc;
	}
	return rv;
}

int all_tasks(long timeout, plc_t p) {
    int i = 0;
    int rv = PLC_OK;
    if(p==NULL)
        return PLC_ERR;
    
    for(;i < p->rungno; i++)
        rv = task(timeout, p, p->rungs[i]);
    
    return rv;
}

/*************************VM*******************************************/

int handle_jmp( const rung_t r, unsigned int * pc) {
    if(r==NULL
    || pc==NULL)
        return PLC_ERR;
        
    instruction_t op;
    if(get(r, *pc, &op) < PLC_OK)
        return ERR_BADOPERAND;
      
    if(op->operation != IL_JMP)
        return ERR_BADOPERATOR; //sanity
    
    if(!(op->modifier==IL_COND
    && r->acc.u == 0))
        *pc = op->operand;
    else 
        (*pc)++;
    return PLC_OK;
}

int handle_set( const instruction_t op,
                const data_t acc,
                BYTE is_bit,                 
                plc_t p ) {
    int r = PLC_OK;
    if(op==NULL
    || p==NULL){
    
        return PLC_ERR;
    }    
    if(op->operation != IL_SET){
    
        return ERR_BADOPERATOR; //sanity
    }    
    if(op->modifier == IL_COND
    && acc.u == FALSE){
    
        return r;
    }        
    switch (op->operand){
    
        case OP_CONTACT:	//set output %QX.Y
            if(!is_bit) {//only gets called when bit is defined
                r = ERR_BADOPERAND;
            } else {
                r = set(p, 
                        BOOL_DQ, 
                        (op->byte) * BYTESIZE + op->bit);
            }
            break;
            
        case OP_START:	//bits are irrelevant
            r = set(p, BOOL_TIMER, op->byte);
            break;
            
        case OP_PULSEIN:	//same here
            r = set(p, BOOL_COUNTER, op->byte);
            break;
            
        default:
            r = ERR_BADOPERAND;
            break;
    }
    return r;
}

int handle_reset(const instruction_t op, 
                 const data_t acc,
                 BYTE is_bit,
                 plc_t p) {
    int r = PLC_OK;
    if(op==NULL
    || p==NULL)
        return PLC_ERR;
    
    if(op->operation != IL_RESET)
        return ERR_BADOPERATOR; //sanity
    
    if(op->modifier == IL_COND
    && acc.u == FALSE)
        return r;
        
    switch (op->operand){
        case OP_CONTACT:	//set output %QX.Y
            if (!is_bit)	//only gets called when bit is defined
                r = ERR_BADOPERAND;
            else
                r = reset(p, BOOL_DQ, (op->byte) * BYTESIZE + op->bit);
            break;
            
        case OP_START:	//bits are irrelevant
                r = reset(p, BOOL_TIMER, op->byte);
            break;
            
        case OP_PULSEIN:	//same here
                r = reset(p, BOOL_COUNTER, op->byte);
            break;
            
        default:
            r = ERR_BADOPERAND;
    }
    return r;
}

int st_out_r( const instruction_t op, 
              double val,
              plc_t p) {
    if(op->byte >= p->naq)
        return ERR_BADOPERAND;
    BYTE i = op->byte;
    p->aq[i].V = val;    
    return PLC_OK;
}

int st_out( const instruction_t op, 
            uint64_t val,
            plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    int i = 0;
    switch(t){
        case T_BOOL:
            if (op->modifier == IL_NEG)
                val = TRUE - BOOL(val);
            if(op->byte >= p->nq)
                r = ERR_BADOPERAND; 
            else    
                r = contact(p, 
                        BOOL_DQ, 
                        (op->byte) * BYTESIZE + op->bit,
                        BOOL(val));
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            if (op->modifier == IL_NEG)
                val = - val;
            if(op->byte + offs >= p->nq)
                r = ERR_BADOPERAND; 
            else for(;i<=offs;i++){
                    p->outputs[op->byte + i] = 
                    (val >> ((offs-i)*BYTESIZE)) % (0x100);    
                }
            
            break;    
                
        default: 
            r = ERR_BADOPERAND;
    }
    return r;    
}

int st_mem_r( const instruction_t op, 
              double val,
              plc_t p) {
    if(op->byte >= p->nmr)
                return ERR_BADOPERAND; 
    p->mr[op->byte].V = val;
   // plc_log("store %lf to m%d", val, op->byte);
    return PLC_OK; 
}
              
int st_mem( const instruction_t op, 
            uint64_t val,
            plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    uint64_t compl = 0x100;
 
    if(op->byte >= p->nm)
                return ERR_BADOPERAND; 
    switch(t){
        case T_BOOL:
             r = contact(p, 
                         BOOL_COUNTER, 
                         op->byte, 
                         BOOL(val));
             break;
             
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
             p->m[op->byte].V = 
                 val & ((compl << (BYTESIZE * offs))-1);
            // plc_log("store 0x%lx to m%d", val, op->byte);
             break;           
                 
        default: 
            return ERR_BADOPERAND;
    }
    return r;    
}

int handle_st(  const instruction_t op, 
                const data_t acc, 
                plc_t p) {
    int r = PLC_OK;
    data_t val = acc;
    if(op==NULL
    || p==NULL)
        return PLC_ERR;
        
    if(op->operation != IL_ST)
        return ERR_BADOPERATOR; //sanity
     
    switch (op->operand){
        case OP_REAL_CONTACT:	    //set output %QX.Y
            r = st_out_r(op, val.r, p);
            break;
        
        case OP_CONTACT:	    //set output %QX.Y
            r = st_out(op, val.u, p);
            break;
            
        case OP_START:	    //bits are irrelevant
            r = contact(p, BOOL_TIMER, op->byte, val.u % 2);
            break;
            
        case OP_REAL_MEMIN:
            r = st_mem_r(op, val.r, p);
            break;     
            
        case OP_PULSEIN:
            r = st_mem(op, val.u, p);
            break;
            
        case OP_WRITE:
            p->command = val.u;
            break;
            
        default:
            r = ERR_BADOPERAND;
    }
    return r;
}

uint64_t ld_bytes(BYTE start, 
                  BYTE offset, 
                  BYTE * arr) {
    uint64_t rv = 0;
    int i = offset;
    for(; i >=0; i--){
        uint64_t u = arr[start + i]; 
        rv +=  u << (BYTESIZE*(offset -i)) ;
    }
    return rv;
}                

int ld_in( const instruction_t op, 
           uint64_t * val,
           plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    uint64_t complement = 0x100;
    
    switch(t){
        case T_BOOL:
            if(op->byte >= p->ni)
                return ERR_BADOPERAND;
            *val = resolve(p, BOOL_DI, 
                            (op->byte) * BYTESIZE + op->bit);
            if (op->modifier == IL_NEG)
                *val = *val?FALSE:TRUE;      
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            if(op->byte + offs >= p->ni)
                return ERR_BADOPERAND;
                
            *val = ld_bytes(op->byte, offs, p->inputs);    
            
            if(op->modifier == IL_NEG)
                *val = (complement << offs*BYTESIZE) - *val;  
            break;
      
        default: 
            return ERR_BADOPERAND;
    }
    return r;    
}
            
int ld_re( const instruction_t op, 
           BYTE * val,
           plc_t p ) {
    int r = PLC_OK;
    int t = get_type(op);
    if(op->byte >= p->ni)
                return ERR_BADOPERAND;
    if(t == T_BOOL)
            *val = re(p, BOOL_DI, (op->byte) * BYTESIZE + op->bit);
    else
            r = ERR_BADOPERAND;
    return r;    
}
                          
int ld_fe( const instruction_t op, 
           BYTE * val,
           plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    if(op->byte >= p->ni)
                return ERR_BADOPERAND;
    if(t == T_BOOL)
            *val = fe(p, BOOL_DI, 
                      (op->byte) * BYTESIZE + op->bit);
    else 
            r = ERR_BADOPERAND;
    return r;    
}              

int ld_in_r( const instruction_t op, 
             double * val,
             plc_t p) {
    if(op->byte >= p->nai)
                return ERR_BADOPERAND;
    BYTE i = op->byte;
    *val = p->ai[i].V;
    return PLC_OK;    
}
            
int ld_out_r( const instruction_t op, 
            double * val,
            plc_t p) {
    if(op->byte >= p->naq)
                return ERR_BADOPERAND;
    BYTE i = op->byte;
    *val = p->aq[i].V;
    return PLC_OK;    
}            
            
int ld_out( const instruction_t op, 
            uint64_t * val,
            plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    BYTE offs = (op->bit / BYTESIZE) - 1;
    uint64_t complement = 0x100;
    switch(t){
        case T_BOOL:
             if(op->byte >= p->nq)
                return ERR_BADOPERAND;
            *val = resolve(p, BOOL_DQ, 
                           (op->byte) * BYTESIZE + op->bit);
            if (op->modifier == IL_NEG)
                *val = *val?FALSE:TRUE;      
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            if(op->byte + offs >= p->nq)
                return ERR_BADOPERAND;
                
            *val = ld_bytes(op->byte, offs, p->outputs);    
            
            if(op->modifier == IL_NEG)
                *val = (complement << offs*BYTESIZE) - *val;  
            break;
        
        default: 
            return ERR_BADOPERAND;
    }
    return r;    
}
            
int ld_mem( const instruction_t op, 
            uint64_t * val,
            plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    if(op->byte >= p->nm)
                return ERR_BADOPERAND;
    int offs = (op->bit / BYTESIZE) - 1;
    uint64_t compl = 0x100;
                
    switch(t){
        case T_BOOL:
            *val = resolve(p, BOOL_COUNTER, op->byte);
            if (op->modifier == IL_NEG)
                *val = (*val)?FALSE:TRUE;      
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            *val = p->m[op->byte].V & ((compl << offs*BYTESIZE) - 1);
            
            if( op->modifier == IL_NEG )
                *val = (compl << offs*BYTESIZE) - *val;
            break;
                
        default: 
            return ERR_BADOPERAND;
    } 
    return r;    
}

int ld_mem_r( const instruction_t op, 
              double * val,
              plc_t p) {
    if(op->byte >= p->nmr)
                return ERR_BADOPERAND;
                
    *val = p->mr[op->byte].V;
     if( op->modifier == IL_NEG )
                *val = - *val;
                
    return PLC_OK;   
}

int ld_timer( const instruction_t op, 
                uint64_t * val,
                plc_t p) {
    int r = PLC_OK;
    int t = get_type(op);
    int offs = (op->bit / BYTESIZE) - 1;
    uint64_t compl = 0x100;
   
/*a convention: bit is irrelevant, 
  but defining it means we are referring to t.Q, otherwise t.V
*/
    if(op->byte >= p->nt)
                return ERR_BADOPERAND;
    switch(t){
        case T_BOOL:
            *val = resolve(p, BOOL_TIMER, op->byte);
            if (op->modifier == IL_NEG)
                *val = *val?FALSE:TRUE;      
     
            break;
            
        case T_BYTE:
        case T_WORD:
        case T_DWORD:
        case T_LWORD:
            *val = p->t[op->byte].V & ((compl << offs*BYTESIZE) - 1);
            
            if( op->modifier == IL_NEG )
                *val = (compl << offs*BYTESIZE) - *val;
            break;
                
        default: 
            r = ERR_BADOPERAND;
    }
    return r;    
}
            
int handle_ld(  const instruction_t op, 
                data_t * acc, 
                plc_t p) {
    int r = 0;
    BYTE edge = 0; 
    if(op==NULL
    || p==NULL
    || acc==NULL)
        return PLC_ERR;
    
    if((op->operation != IL_LD 
    && op->operation < FIRST_BITWISE)  
    || op->operation >= N_IL_INSN)
        return ERR_BADOPERATOR; //sanity
        
    switch (op->operand){
        case OP_OUTPUT:	//set output %QX.Y
            r = ld_out(op, &(acc->u), p);
            break; 
            
        case OP_INPUT:	//load input %IX.Y
            r = ld_in(op, &(acc->u), p);
            break;
        
        case OP_REAL_OUTPUT:	//set output %QX.Y
            r = ld_out_r(op, &(acc->r), p);
            break; 
            
        case OP_REAL_INPUT:	//load input %IX.Y
            r = ld_in_r(op, &(acc->r), p);
            break;
            
        case OP_MEMORY:
            r = ld_mem(op, &(acc->u), p);
            break;
            
        case OP_REAL_MEMORY:
            r = ld_mem_r(op, &(acc->r), p);
            break;    
            
        case OP_TIMEOUT:
            r = ld_timer(op, &(acc->u), p);
            break;
            
        case OP_BLINKOUT:	//bit is irrelevant
            if(op->byte >= p->ns)
                return ERR_BADOPERAND;
            acc->u = resolve(p, BOOL_BLINKER, op->byte);
            break;
            
        case OP_COMMAND:
            acc->u = p->command;
            break;
            
        case OP_RISING:	//only boolean
            r = ld_re(op, &edge, p);
            acc->u = edge;
            break;
            
        case OP_FALLING:	//only boolean
            r = ld_fe(op, &edge, p);
            acc->u = edge;
            break;
            
        default:
            r = ERR_BADOPERAND;
            break;
    }
    return r;
}

int handle_stackable(   const instruction_t op, 
                        rung_t r,  
                        plc_t p)
{//all others (stackable operations)
    int rv = 0;
    data_t val;
    val.u = 0;
    BYTE stackable = 0;
    if(r==NULL
    || p==NULL)
        return PLC_ERR;
    
    if(op->operation < FIRST_BITWISE 
    || op->operation >=  N_IL_INSN)
        return ERR_BADOPERATOR; //sanity
    
    int type = get_type(op);
    if(type == PLC_ERR)
        return ERR_BADOPERAND;
    
    struct instruction loader;
    deepcopy(op, &loader);
    loader.operation = IL_LD;
    loader.modifier = IL_NORM;
    
    stackable = op->operation;
    
    if (op->modifier == IL_NEG)
        stackable += NEGATE;
        
    if (op->modifier == IL_PUSH){
        push(stackable, type, r->acc, r); 
        rv = handle_ld(&loader,  &(r->acc), p);	   
    }
    else{    
        rv = handle_ld( &loader, &val, p);
        r->acc = operate(stackable, type, r->acc, val);
    }
    return rv;
}

int instruct(plc_t p, rung_t r, unsigned int *pc)
{
    BYTE type = 0;
    int error = 0;
    instruction_t op;
    BYTE increment = TRUE;
    if(r==NULL
    || p==NULL
    || *pc >= r->insno){   
        (*pc)++;
        return PLC_ERR;
    }
    if(get(r, *pc , &op) < PLC_OK){   
        (*pc)++;
        return ERR_BADOPERAND;
    }
    
    type = get_type(op);
    if(type == PLC_ERR)
        return ERR_BADOPERAND;
    /*
    char dump[MAXSTR] = "";
    dump_instruction(op, dump);
    plc_log("%d.%s", *pc, dump);
    */
	switch (op->operation){
//IL OPCODES: no operand
	case IL_POP: //POP
		r->acc = pop(r->acc, &(r->stack));
		break;
	case IL_NOP: 
//null operation	
	case IL_CAL: 
//subroutine call (unimplemented) retrieve subroutine line, set pc
	case IL_RET: 
//unimplemented yet: retrieve  previous program , counter, set pc
		break;
//arithmetic LABEL
	case IL_JMP:			//JMP
        error = handle_jmp(r, pc);
        increment = FALSE;
//retrieve line number from label, set pc
		break;
//boolean, no modifier, outputs.
	case IL_SET:	//S
        error = handle_set( op, 
                            r->acc,//.u % 0x100, 
                            type == T_BOOL, 
                            p);
		break;
	case IL_RESET:	//R
        error = handle_reset( op,                                 
                              r->acc,//.u % 0x100, 
                              type == T_BOOL,
                              p);
		break;
    case IL_LD:	//LD
        error = handle_ld( op, &(r->acc), p);
        
		break;
	case IL_ST:	//ST: output
		//if negate, negate acc
        error = handle_st( op, r->acc, p);
//any operand, only push
		break;
    default:
        error = handle_stackable( op,  r,  p);
	}
	if(increment == TRUE)
	    (*pc)++;
    return error;
}

rung_t mk_rung(const char * name, plc_t p) {
    rung_t r = (rung_t)malloc(sizeof(struct rung));
    memset(r, 0, sizeof(struct rung));
    r->id = strdup(name);
    if(p->rungs == NULL){//lazy allocation
       p->rungs = (rung_t *)malloc(MAXRUNG*sizeof(rung_t));
       memset(p->rungs, 0, MAXRUNG*sizeof(rung_t));
    }
    p->rungs[p->rungno++] = r;
    
    return r;
}

rung_t get_rung(const plc_t p, const unsigned int idx) {
    if(p==NULL
    || idx >= p->rungno){ 
        return NULL;
    } 
    return p->rungs[idx];
}

/*****************realtime loop************************************/
int timeval_subtract(struct timeval *result, 
                     struct timeval *x,
		             struct timeval *y) { 
/* Subtract the `struct timeval' values X and Y,
 storing the result in RESULT.
 Return 1 if the difference is negative, otherwise 0.  */
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec){
		int nsec = (y->tv_usec - x->tv_usec) / MILLION + 1;
		y->tv_usec -= MILLION * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > MILLION){
		int nsec = (x->tv_usec - y->tv_usec) / MILLION;
		y->tv_usec += MILLION * nsec;
		y->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void read_inputs(plc_t p) {
    int i=0;
    int n=0;
    int j=0;
    
    BYTE i_bit = 0;
    
    if(p == NULL
    || p->hw == NULL)
        return;
    
    p->hw->fetch();//for simulation
    
    for (i = 0; i < p->ni; i++){	//for each input byte
        p->inputs[i] = 0;
        for (j = 0; j < BYTESIZE; j++){	//read n bit into in
            n = i * BYTESIZE + j;
            i_bit = 0;
            p->hw->dio_read(n, &i_bit);
            p->inputs[i] |= i_bit << j;
        }	//mask them
    }
    
    for (i = 0; i < p->nai; i++){	//for each input sample
        p->hw->data_read(i, &p->real_in[i]);
    }
}

void write_outputs(plc_t p) {
    int j=0;
    int n=0;
    int q_bit=0;
    
    int i=0;
    if(p == NULL
    || p->hw == NULL)
        return;
    
    for (i = 0; i < p->nq; i++){	
        for (j = 0; j < BYTESIZE; j++){	//write n bit out
            n = BYTESIZE * i + j;
            q_bit = (p->outputs[i] >> j) % 2;
            p->hw->dio_write(p->outputs, n, q_bit);
        }
    }
    for (i = 0; i < p->naq; i++){	//for each output sample
        p->hw->data_write(i, p->real_out[i]);
    }
    p->hw->flush();//for simulation
}

int is_forced(const plc_t p, int op, BYTE i) {
    int r = PLC_ERR;
    switch(op){
        case OP_REAL_INPUT:if(i < p->nai){
                                r = p->ai[i].mask < p->ai[i].max
                                && p->ai[i].mask > p->ai[i].min;
                           }
        case OP_REAL_OUTPUT:if(i < p->naq){
                                r = p->ai[i].mask < p->ai[i].max
                                && p->ai[i].mask > p->ai[i].min;
                            }                    
        default:break;  
    }
    return r;
}

BYTE dec_inp(plc_t p) { //decode input bytes
	BYTE i = 0;
	BYTE j = 0;
	BYTE i_changed = FALSE;
	
	for (; i < p->ni; i++){
        if (p->inputs[i] != p->old->inputs[i]){
            i_changed = TRUE;
        }
	    for(; j < BYTESIZE; j++){
	        unsigned int n = BYTESIZE * i + j;
	        
		    p->di[n].I = (((p->inputs[i] >> j) % 2) 
		                 || p->di[n].SET) && !p->di[n].RESET;
		    BYTE edge = p->di[n].I ^ p->old->di[n].I;             
		    p->di[n].RE = p->di[n].I && edge;
		    p->di[n].FE = !p->di[n].I && edge;
	    }
	}
	for (i = 0; i < p->nai; i++){
	    if(is_forced(p, OP_REAL_INPUT, i)){
            p->ai[i].V = p->ai[i].mask;
        } else {
	        double denom = (double)UINT64_MAX;   
            double v = p->real_in[i]; 
            double min = p->ai[i].min;
            double max = p->ai[i].max;
            p->ai[i].V = min + ((max - min) * (v/denom));
        }
        if (abs(p->ai[i].V - p->old->ai[i].V) > FLOAT_PRECISION){
            i_changed = TRUE;
        }
	}
	memset(p->outputs, 0, p->nq);
	return i_changed;
}

BYTE enc_out(plc_t p) { //encode digital outputs to output bytes
	BYTE i = 0;
	BYTE j = 0;
	BYTE o_changed=FALSE;
	BYTE out[p->nq];
	
	memcpy(out, p->outputs, p->nq);
	
	for (; i < p->nq ; i++){//write masked outputs
        for(; j < BYTESIZE; j++){
            unsigned int n = BYTESIZE * i + j;
            if(p->dq[n].MASK){
                out[i] |= (p->dq[n].SET && !p->dq[n].RESET)
                       << j;
            } else {
                out[i] |= (p->dq[n].Q || (p->dq[n].SET && !p->dq[n].RESET))
				       << j;
		    }
	    }	    
	    p->outputs[i] = out[i];
	    if (p->outputs[i] != p->old->outputs[i]){
            o_changed = TRUE;
	    }
	}	
	for (i = 0; i < p->naq; i++){
	    double min = p->aq[i].min;
        double max = p->aq[i].max;
        double val = p->aq[i].V;
        if(is_forced(p, OP_REAL_OUTPUT, i)){
            val = p->aq[i].mask;    
        }
        p->real_out[i] = UINT64_MAX * ((val - min)/(max-min));
        
        if (abs(p->aq[i].V - p->old->aq[i].V) > FLOAT_PRECISION){
            o_changed = TRUE;
        }
    }
    return o_changed;
}

void read_mvars(plc_t p) {
	int i;
	for (i = 0; i < p->nm; i++){
		if (p->m[i].SET || p->m[i].RESET)
			p->m[i].PULSE = p->m[i].SET && !p->m[i].RESET;
	}
}

void write_mvars(plc_t p) {
	int i;
	for (i = 0; i < p->nm; i++){
		if (!p->m[i].RO){
			if (p->m[i].PULSE && p->m[i].EDGE) {//up/down counting
				p->m[i].V += (p->m[i].DOWN) ? -1 : 1;
				p->m[i].EDGE = FALSE;
			}
		}
	}
}

BYTE check_pulses(plc_t p) {
    BYTE changed=0;
    int i=0;
    for (i = 0; i < p->nm; i++){//check counter pulses
        if (p->m[i].PULSE != p->old->m[i].PULSE){
            p->m[i].EDGE = TRUE;
            changed = TRUE;
        }
    }
    return changed;
}

plc_t save_state(BYTE mask,
                plc_t p) {
    if (mask & CHANGED_I) {// Input changed!
        memcpy(p->old->inputs, p->inputs, p->ni);
        plc_log("%s", "input updated"); 
    }
    if (mask & CHANGED_O) {// Output changed!"
        memcpy(p->old->outputs, p->outputs, p->nq);
        plc_log("%s", "output updated"); 
    }
    if (mask & CHANGED_M) {
        memcpy(p->old->m, p->m, p->nm * sizeof(struct mvar));
        plc_log("%s", "regs updated"); 
    }
    if (mask & CHANGED_T) {
        memcpy(p->old->t, p->t, p->nt * sizeof(struct timer));
        plc_log("%s", "timers updated"); 
    }
    if (mask & CHANGED_S) {
        memcpy(p->old->s, p->s, p->ns * sizeof(struct blink));
        plc_log("%s", "pulses updated"); 
    }
    p->update = mask;
    return p;
}

void write_response(plc_t p) {
    int rfd = 0; //response file descriptor
    rfd = open(p->response_file, O_NONBLOCK | O_WRONLY);
    //dummy code until this feature goes away
    if(write(rfd, &(p->response), 1) < 0)
        p->response = PLC_ERR;
    else 
        p->response = PLC_OK;
    close(rfd);
}

BYTE manage_timers(plc_t p) {
    int i=0;
    BYTE t_changed = 0;
    for (i = 0; i < p->nt; i++){
        if (p->t[i].V < p->t[i].P && p->t[i].START){
            if (p->t[i].sn < p->t[i].S)
                p->t[i].sn++;
            else{
                t_changed = TRUE;
                p->t[i].V++;
                p->t[i].sn = 0;
            }
            p->t[i].Q = (p->t[i].ONDELAY) ? 0 : 1;	//on delay
        }
        else if (p->t[i].START){
            p->t[i].Q = (p->t[i].ONDELAY) ? 1 : 0;	//on delay
        }
    }
    return t_changed;
}

BYTE manage_blinkers(plc_t p) {
    BYTE s_changed=0;
    int i=0;
    for (i = 0; i < p->ns; i++){
        if (p->s[i].S > 0){	//if set up
            if (p->s[i].sn > p->s[i].S){
                s_changed = TRUE;
                p->s[i].Q = (p->s[i].Q) ? 0 : 1;	//toggle
                p->s[i].sn = 0;
            }
            else
                p->s[i].sn++;
        }
    }
    return s_changed;
}

plc_t plc_load_program_file(const char * path, plc_t plc) {
    FILE * f;
    int r = ERR_BADFILE;
    char program_lines[MAXBUF][MAXSTR];///program lines
    char line[MAXSTR];
    int i=0;
    
    if(path == NULL){
    
        return plc;
    }
    char * ext = strrchr(path, '.');
    int lang = PLC_ERR;
    
    if(ext != NULL){
        if(strcmp(ext, ".il") == 0){
            lang = LANG_IL;
        }else if(strcmp(ext, ".ld") == 0) {
            lang = LANG_LD;
        }
    }          
    if (lang > PLC_ERR && (f = fopen(path, "r"))) {
        memset(line, 0, MAXSTR);
       
        while (fgets(line, MAXSTR, f)) {
            memset(program_lines[i], 0, MAXSTR);
            sprintf(program_lines[i++], "%s\n", line);
        }
        r = PLC_OK;
    } 
    if(r > PLC_ERR){
        if(lang == LANG_IL){
            plc_log("Loading IL code from %s...", path);
            plc = parse_il_program(path, program_lines, plc);
        }else{ 
            plc_log("Loading LD code from %s...", path);
            plc = parse_ld_program(path, program_lines, plc);   
        }
    } else {
        plc->status = r;
    }
    return plc;
}

plc_t plc_start(plc_t p){
    
    if(p == NULL
    || p->hw == NULL)
        return NULL;
    
    p->hw->enable();
        
    if(p->status == ST_STOPPED){
        p->update = CHANGED_STATUS;
    }
    p->status = ST_RUNNING;
    
    return p;
}

plc_t plc_stop(plc_t p){
    if(p == NULL
    || p->hw == NULL)
        return NULL;
        
    memset(p->outputs, 0, p->nq);
    memset(p->real_out, 0, 8*p->naq);
    write_outputs(p);
        
    p->hw->disable();    
    if(p->status == ST_RUNNING){
        p->update = CHANGED_STATUS;
    }
    p->status = ST_STOPPED;
    
    return p;
}

plc_t plc_func(plc_t p) {
	struct timeval tp; //time for poll
	struct timeval tn; //time since beginning of last output
	struct timeval dt;
    long timeout = p->step * THOUSAND; //timeout in usec
    long poll_time = 0;
    long io_time = 0;
	static long run_time = 0;
	int written=FALSE;
    int r = PLC_OK;
    BYTE change_mask = p->update;
	BYTE i_changed = FALSE;
	BYTE o_changed = FALSE;
	BYTE m_changed = FALSE;
	BYTE t_changed = FALSE;
	BYTE s_changed = FALSE;
    //char test[NICKLEN];
    dt.tv_sec = 0;
    dt.tv_usec = 0;
	if ((p->status) == ST_RUNNING){//run
//remaining time = step 
        read_inputs(p);
        t_changed = manage_timers(p);
        s_changed = manage_blinkers(p);
        read_mvars(p);
        
        gettimeofday(&tn,NULL);
//dt = time for input + output	
//how much time passed since previous cycle?
        timeval_subtract(&dt, &tn, &Curtime);
        dt.tv_usec = dt.tv_usec % (THOUSAND * p->step);
        io_time = dt.tv_usec;// / THOUSAND;
        timeout -= io_time;
        timeout -= run_time;
//plc_log("I/O time approx:%d microseconds",dt.tv_usec);
//poll on plcpipe for command, for max STEP msecs
        written = poll(p->com, 0, timeout / THOUSAND);
//TODO: when a truly asunchronous UI is available, 
//replace poll() with sleep() for better accuracy
        gettimeofday(&tp, NULL);	//how much time did poll wait?
        timeval_subtract(&dt, &tp, &tn);
        poll_time =  dt.tv_usec;
//plc_log("Poll time approx:%d microseconds",dt.tv_usec);
//dt = time(input) + time(poll)
        
        if (written<0){
		    r = PLC_ERR;
            plc_log("PIPE ERROR\n");
			p->command = 0;
		}
        i_changed = dec_inp(p); //decode inputs
//TODO: a better user plugin system when function blocks are implemented
		project_task(p); //plugin code

        if(r >= PLC_OK)
            r = all_tasks(p->step * THOUSAND, p);
                
        gettimeofday(&Curtime, NULL);	//start timing next cycle
        timeval_subtract(&dt, &Curtime, &tp);
        run_time =  dt.tv_usec;
        compute_variance((double)(run_time + poll_time + io_time));
        
        if(r == ERR_TIMEOUT){    
            plc_log("timeout! i/o: %d us, poll: %d us, run: %d us",
                    io_time, poll_time, run_time);
        }
        o_changed = enc_out(p);
		p->command = 0;

        write_outputs(p);

        m_changed = check_pulses(p);
        write_mvars(p);
        change_mask |= CHANGED_I * i_changed;
        change_mask |= CHANGED_O * o_changed;
        change_mask |= CHANGED_M * m_changed;
        change_mask |= CHANGED_T * t_changed;
        change_mask |= CHANGED_S * s_changed;
        p = save_state(change_mask, p);
	}
    else{
        usleep(p->step * THOUSAND);
        timeout = 0;
    }
    if(r < PLC_OK){
        p->status = r;
    }
    return p;
}

static plc_t allocate(plc_t plc) {
/*******************initialize***************/    

    plc->inputs = (BYTE *) malloc(plc->ni);
    plc->outputs = (BYTE *) malloc(plc->nq);
    plc->real_in = (uint64_t *) malloc(plc->nai * sizeof(uint64_t));
    plc->real_out = (uint64_t *) malloc(plc->naq * sizeof(uint64_t));
    plc->di = (di_t) malloc(
            BYTESIZE * plc->ni * sizeof(struct digital_input));
    plc->dq = (do_t) malloc(
            BYTESIZE * plc->nq * sizeof(struct digital_output));
    
    plc->t = (dt_t) malloc(plc->nt * sizeof(struct timer));
    plc->s = (blink_t) malloc(plc->ns * sizeof(struct blink));
    plc->m = (mvar_t) malloc(plc->nm * sizeof(struct mvar));
    plc->mr = (mreal_t) malloc(plc->nmr * sizeof(struct mreal));
   
    plc->ai = (aio_t) malloc(
             plc->nai * sizeof(struct analog_io));
    plc->aq = (aio_t) malloc(
             plc->naq * sizeof(struct analog_io));
   
    memset(plc->real_in, 0, plc->nai*sizeof(uint64_t));
    memset(plc->real_out, 0, plc->naq*sizeof(uint64_t));
    memset(plc->inputs, 0, plc->ni);
    memset(plc->outputs, 0, plc->nq);
  
    memset(plc->di, 0, BYTESIZE * plc->ni * sizeof(struct digital_input));
    memset(plc->dq, 0, BYTESIZE * plc->nq * sizeof(struct digital_output));
    memset(plc->t, 0, plc->nt * sizeof(struct timer));
    memset(plc->s, 0, plc->ns * sizeof(struct blink));
    memset(plc->m, 0, plc->nm * sizeof(struct mvar));
    memset(plc->mr, 0, plc->nmr * sizeof(struct mreal));
    
    return plc;
}

/***************construct*******************/
plc_t new_plc(
    int di, 
    int dq,
    int ai,
    int aq,
    int nt, 
    int ns,
    int nm,
    int nr,
    int step,
    hardware_t hw){

    plc_t plc = (plc_t)malloc(sizeof(struct PLC_regs));
    memset(plc, 0, sizeof(struct PLC_regs));
    
    plc->ni = di;
    plc->nq = dq;
    plc->nai = ai;
    plc->naq = aq;
    plc->nt = nt;
    plc->ns = ns;
    plc->nm = nm;
    plc->nmr = nr;
    
    plc->hw = hw;
    plc->step = step ;
    
    plc->command = 0;
    plc->status = ST_RUNNING;
  
    plc = allocate(plc);
    
    plc->old = copy_plc(plc);
  
    return plc;
}

plc_t copy_plc(const plc_t plc) {

    plc_t p = (plc_t)malloc(sizeof(struct PLC_regs));
    memset(p, 0, sizeof(struct PLC_regs));
    p->ni = plc->ni;
    p->nq = plc->nq;
    p->nai = plc->nai;
    p->naq = plc->naq;
    p->nt = plc->nt;
    p->ns = plc->ns;
    p->nm = plc->nm;
    p->nmr = plc->nmr;
    
    p = allocate(p);
    
    memcpy(p->inputs, plc->inputs, plc->ni);
    memcpy(p->outputs, plc->outputs, plc->nq);
    memset(p->real_in, 0, plc->nai*sizeof(uint64_t));
    memset(p->real_out, 0, plc->naq*sizeof(uint64_t));
    
    memcpy(p->m, plc->m, plc->nm * sizeof(struct mvar));
    memcpy(p->mr, plc->mr, plc->nmr * sizeof(struct mreal));
    memcpy(p->t, plc->t, plc->nt * sizeof(struct timer));
    memcpy(p->s, plc->s, plc->ns * sizeof(struct blink));
    
    return p;
}
/*destroy*/
void clear_plc(plc_t plc){
    if(plc != NULL){
        if(plc->ai !=NULL){
            free(plc->ai );
        }
        if(plc->aq!=NULL){
            free(plc->aq);
        }
        if(plc->mr !=NULL){
            free(plc->mr );
        }
        if(plc->m!=NULL){
            free(plc->m);
        }
        if( plc->s !=NULL){
            free( plc->s );
        }
        if(plc->t !=NULL){
            free(plc->t );
        }
        if(plc->dq!=NULL){
            free(plc->dq);
        }
        if(plc->di  !=NULL){
            free(plc->di  );
        }
        if(plc->real_out !=NULL){
            free(plc->real_out );
        }
        if(plc->real_in!=NULL){
            free(plc->real_in);
        }
        if(plc->outputs!=NULL){
            free(plc->outputs);
        }
        if(plc->inputs!=NULL){
            free(plc->inputs);
        }
        free(plc);    
    }
}
/*configurators*/
plc_t declare_variable(const plc_t p, 
                        int var, 
                        BYTE idx, 
                        const char* val) {
    plc_t r = p;
    char * nick = NULL;
    BYTE max = 0;
    switch(var){
        case OP_INPUT:
            max = p->ni*BYTESIZE;
            nick = r->di[idx].nick;
            break;
            
        case OP_OUTPUT:
            max = p->nq*BYTESIZE;
            nick = r->dq[idx].nick;
            break;
            
        case OP_REAL_INPUT:
            max = p->nai;
            nick = r->ai[idx].nick;
            break;
            
        case OP_REAL_OUTPUT:
            max = p->naq;
            nick = r->aq[idx].nick;
            break;    
            
        case OP_MEMORY:
            max = p->nm;
            nick = r->m[idx].nick;
            break;
            
        case OP_REAL_MEMORY:
            max = p->nmr;
            nick = r->mr[idx].nick;
            break;
            
        case OP_TIMEOUT:
            max = p->nt;
            nick = r->t[idx].nick;
            break;
            
        case OP_BLINKOUT:
            max = p->ns;
            nick = r->s[idx].nick;
            break;        
            
        default: break;
    }
    
    if(nick == NULL){
        
        r->status = ERR_BADOPERAND;
    } else if(idx >= max){
        
        r->status = ERR_BADINDEX;
    } else {        
        
        snprintf(nick, NICKLEN, "%s", val);
    }
    return r;
}

plc_t init_variable(const plc_t p, int var, BYTE idx, const char* val){
    plc_t r = p;
    BYTE len = 0;

    switch(var){
       case OP_REAL_MEMORY:
            len = r->nmr;
            if(idx >= len){
                r->status = ERR_BADINDEX;
            } else {
                r->mr[idx].V = atof(val);
            }
            break;
            
        case OP_MEMORY:
            len = r->nm;
            if(idx >= len){
                r->status = ERR_BADINDEX;
            } else {
                r->m[idx].V = atol(val);
            }
            break;    
        
        default:
            r->status = ERR_BADOPERAND; 
            break;
    }
    return r;
}

plc_t configure_variable_readonly(const plc_t p, 
                                int var, 
                                BYTE idx, 
                                const char* val){
    plc_t r = p;
    BYTE len = 0;
    switch(var){
       case OP_REAL_MEMORY:
            len = r->nmr;
            if(idx >= len){
                r->status = ERR_BADINDEX;
            } else {
                r->mr[idx].RO = !strcmp(val, "TRUE");
            }
            break;
            
        case OP_MEMORY:
            len = r->nm;
            if(idx >= len){
                r->status = ERR_BADINDEX;
            } else {
                r->m[idx].RO = !strcmp(val, "TRUE");
            }
            break;    
        
        default:
            r->status = ERR_BADOPERAND; 
            break;
    }
    return r;
}

plc_t configure_io_limit(const plc_t p, 
                        int var, 
                        BYTE idx, 
                        const char* val,
                        BYTE upper){
    plc_t r = p;
    aio_t io = NULL;
    BYTE len = 0;
    switch(var){
        case OP_REAL_INPUT:
            io = r->ai;
            len = r->nai;
            break;
            
        case OP_REAL_OUTPUT:
            io = r->aq;
            len = r->naq;
            break;    
        
        default: break;
    }
    
    if(io == NULL){
        
        r->status = ERR_BADOPERAND;
    } else if(idx >= len){
        
        r->status = ERR_BADINDEX;
    } else if(upper) {        
        
        io[idx].max = atof(val);
    } else {
        
        io[idx].min = atof(val);
    }    
    return r;
}

plc_t configure_counter_direction(const plc_t p, 
                                    BYTE idx, 
                                    const char* val){
    plc_t r = p;
    BYTE len = r->nm;
    
    if(idx >= len){
        r->status = ERR_BADINDEX;
    } else {
        r->m[idx].DOWN = !strcmp(val, "DOWN");
    }
    return r;
}

plc_t configure_timer_scale(const plc_t p, 
                     BYTE idx, 
                     const char* val){
    plc_t r = p;
    BYTE len = r->nt;
    
    if(idx >= len){
        r->status = ERR_BADINDEX;
    } else {
        r->t[idx].S = atol(val);
    }
    return r;
}
           
plc_t configure_timer_preset(const plc_t p, 
                        BYTE idx, 
                        const char* val){
    plc_t r = p;
    BYTE len = r->nt;

    if(idx >= len){
        r->status = ERR_BADINDEX;
    } else {
        r->t[idx].P = atol(val);
    }
    return r;
}                     

plc_t configure_timer_delay_mode(const plc_t p, 
                        BYTE idx, 
                        const char* val){
    plc_t r = p;
    BYTE len = r->nt;
     if(idx >= len){
        r->status = ERR_BADINDEX;
    } else {
        r->t[idx].ONDELAY = !strcmp(val, "ON");
    }
    return r;
}

plc_t configure_pulse_scale(const plc_t p, 
                     BYTE idx, 
                     const char* val){
    plc_t r = p;
    BYTE len = r->ns;
    
    if(idx >= len){
        r->status = ERR_BADINDEX;
    } else {
        r->s[idx].S = atol(val);
    }
    return r;
}

