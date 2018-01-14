#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#include "plcemu.h"
#include "config.h"
#include "util.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "parser-tree.h"
#include "parser-il.h"
#include "parser-ld.h"
#include "ui.h"
#include "hardware.h"

/*************GLOBALS************************************************/
plc_t Plc;
config_t Conf;
//int UiReady=FALSE;

typedef enum {
    MSG_PLCERR,
    MSG_BADCHAR,
    MSG_BADFILE,
    MSG_BADOPERAND,
    MSG_BADINDEX,
    MSG_BADCOIL,
    MSG_BADOPERATOR,
    MSG_TIMEOUT,
    MSG_OVFLOW,
    N_ERRMSG
}ERRORMESSAGES;

const char ErrMsg[N_ERRMSG][MEDSTR] = {
        "Something went terribly wrong!",
        "Invalid Symbol!",
        "Wrong File!",
        "Invalid Operand!",
        "Invalid Numeric!",
        "Invalid Output!",
        "Invalid Command!",
        "Timeout!",
        "Stack overflow!"
};

const char LangStr[3][TINYSTR] ={
     "Ladder Diagram",
    "Instruction List",
    "Structured Text"
};


void sigenable() {
    ui_toggle_enabled();
}

/**
* @brief graceful shutdown
*/
void sigkill() {
    plc_log("program interrupted");
    double mean, var = 0;
    unsigned long loop = get_loop();
    get_variance(&mean, &var);
    plc_log("Total loops: %d", loop);
    plc_log("Average loop time: %f us", mean);
    plc_log("Standard deviation: +-%f us", sqrt(var));
    ui_end();
    exit(0);
}

plc_t declare_names(int operand,
                    const variable_t var,                      
                    plc_t plc){

    char * name = var->name;
    if(name){
        unsigned char index = var->index;
        char * byte = get_param_val("BYTE", var->params);
        if(byte){
            int idx = atoi(byte) * BYTESIZE;
            char * bit = get_param_val("BIT", var->params);
            if(bit){
                idx += atoi(bit);
            } 
       
            if(idx>=0 && idx < 256){
                index = (unsigned char)idx;
            }
        }
        return declare_variable(plc, operand, index, name); 
    } else return plc;
} 

plc_t configure_limits(int operand,                        
                       const variable_t var, 
                       plc_t plc){
    plc_t p = plc;
    char * max = NULL;
    char * min = NULL;
    
    if((max = get_param_val("MAX", var->params))){
        p = configure_io_limit(p, operand, var->index, max, TRUE);
    }
    if((min = get_param_val("MIN", var->params))){
        p = configure_io_limit(p, operand, var->index, min, FALSE);
    }   
    return p;                         
}

plc_t init_values(int operand,                        
                  const variable_t var, 
                  plc_t plc){
    char * val = NULL;
    
    if((val = get_param_val("VALUE", var->params))){
        return init_variable(plc, operand, var->index, val);
    }
    return plc;                         
}

plc_t configure_readonly(int operand,                        
                        const variable_t var, 
                        plc_t plc){
    char * val = NULL;
    
    if((val = get_param_val("READONLY", var->params))){
      return configure_variable_readonly(plc, operand, var->index, val);
    } 
    return plc;               
}

plc_t configure_di(const config_t conf, plc_t plc){
 
    sequence_t seq = get_sequence_entry(CONFIG_DI, conf);
   
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
        //names
            p = declare_names(OP_INPUT, &(seq->vars[i]), p);    
        }
        return p;
    } 
    return plc;
}

plc_t configure_dq(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_DQ, conf); 
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
        //names
            p = declare_names(OP_OUTPUT, &(seq->vars[i]), p);    
        }
        return p;
    } 
    return plc;
}

plc_t configure_ai(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_AI, conf);
    
    if(seq) {
        
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_REAL_INPUT, &(seq->vars[i]), p);
            //limits    
            p = configure_limits(OP_REAL_INPUT, &(seq->vars[i]), p);
        }
        return p;
    } 
    return plc;
}

plc_t configure_aq(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_AQ, conf);
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_REAL_OUTPUT, &(seq->vars[i]), p);
            //limits    
            p = configure_limits(OP_REAL_OUTPUT, &(seq->vars[i]), p);
        }
        return p;
    } 
    return plc;
}

plc_t configure_counters(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_MREG, conf);
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        char * val = NULL;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_MEMORY, &(seq->vars[i]), p);
            //defaults
            p = init_values(OP_MEMORY, &(seq->vars[i]), p);
            //readonlies
            p = configure_readonly(OP_MEMORY, &(seq->vars[i]), p);
            //directions    
            if((val = get_param_val("COUNT", seq->vars[i].params))){
                p = configure_counter_direction(p, i, val);
                val = NULL;
            }
        }
        return p;   
    } 
    return plc;
}

plc_t configure_reals(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_MVAR, conf);
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_REAL_MEMORY, &(seq->vars[i]), plc);
            //defaults
            p = init_values(OP_REAL_MEMORY, &(seq->vars[i]), p);
            //readonlies
            p = configure_readonly(OP_REAL_MEMORY, &(seq->vars[i]), p); 
        }
        return p; 
    } 
    return plc;
}

plc_t configure_timers(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_TIMER, conf);
    
    if(seq) {
        int i = 0;
        char * scale = NULL;
        char * preset = NULL;
        char * ondelay = NULL;
        plc_t p = plc;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_TIMEOUT, &(seq->vars[i]), plc);
            //scales
            if((scale = get_param_val("RESOLUTION", 
                                        seq->vars[i].params))){
                p = configure_timer_scale(p, i, scale);
                scale = NULL;
            }
            //presets            
            if((preset = get_param_val("PRESET", 
                                        seq->vars[i].params))){
                p = configure_timer_preset(p, i, preset);
                preset = NULL;
            }
            //modes
            if((ondelay = get_param_val("ONDELAY", 
                                        seq->vars[i].params))){
                p = configure_timer_delay_mode(p, i, ondelay);
                ondelay = NULL;
            }
        }   
        return p; 
    } 
    return plc;    
}

plc_t configure_pulses(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_PULSE, conf);
    
    if(seq) {
        int i = 0;
        char * scale = NULL;
        plc_t p = plc;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_BLINKOUT, &(seq->vars[i]), plc);
            //scales
            if((scale = get_param_val("RESOLUTION",
                                     seq->vars[i].params))){
                p = configure_pulse_scale(p, i, scale);
                scale = NULL;
            }
        }   
        return p; 
    } 
    return plc;    
}


plc_t configure(const config_t conf, plc_t plc){

    plc_t p = plc;
    
    p = configure_di(conf, p);
    p = configure_dq(conf, p);
    p = configure_ai(conf, p);
    p = configure_aq(conf, p);
    p = configure_counters(conf, p);
    p = configure_reals(conf, p);
    p = configure_timers(conf, p);
    p = configure_pulses(conf, p);
    
    return p;
}

plc_t init_emu(const config_t conf) {
   
    hw_config(conf);
        
    int di = get_sequence_entry(CONFIG_DI, conf)->size / BYTESIZE + 1; 
    int dq = get_sequence_entry(CONFIG_DQ, conf)->size / BYTESIZE + 1;
    int ai = get_sequence_entry(CONFIG_AI, conf)->size;
    int aq = get_sequence_entry(CONFIG_AQ, conf)->size;
    int nt = get_sequence_entry(CONFIG_TIMER, conf)->size;
    int ns = get_sequence_entry(CONFIG_PULSE, conf)->size;
    int nm = get_sequence_entry(CONFIG_MREG, conf)->size;
    int nr = get_sequence_entry(CONFIG_MVAR, conf)->size;
    int step = get_numeric_entry(CONFIG_STEP, conf);
    char * hw = get_string_entry(CONFIG_HW, conf);
    
    plc_t p = new_plc(di, dq, ai, aq, nt, ns, nm, nr, step, hw);
    p = configure(conf, p);
    p->status = 0;
    p->update = TRUE;

    //signal(conf->sigenable, sigenable);
    signal(SIGINT, sigkill);
    signal(SIGTERM, sigkill);
    
    project_init();
    
    open_pipe(get_string_entry(CONFIG_HW, conf), p);
    return p;
}

const char * Usage = "Usage: plcemu [-c config file] \n \
        Options:\n \
        -h displays this help message\n \
        -c uses a configuration file other than config.yml";

void print_error(int errcode)
{
    const char * errmsg;
    switch(errcode) {
        case ERR_BADCHAR:
            errmsg = ErrMsg[MSG_BADCHAR];
            break;
        case ERR_BADFILE:
            errmsg = ErrMsg[MSG_BADFILE];
            break;
        case ERR_BADOPERAND:
            errmsg = ErrMsg[MSG_BADOPERAND];
            break;
        case ERR_BADINDEX:
            errmsg = ErrMsg[MSG_BADINDEX];
            break; 
        case ERR_BADCOIL:
            errmsg = ErrMsg[MSG_BADCOIL];
            break;
        case ERR_BADOPERATOR:
            errmsg = ErrMsg[MSG_BADOPERATOR];
            break;
        case ERR_TIMEOUT:
            errmsg = ErrMsg[MSG_TIMEOUT];
            break;   
        case ERR_OVFLOW:
            errmsg = ErrMsg[MSG_OVFLOW];
            break;                 
        default://PLC_ERR
            errmsg = ErrMsg[MSG_PLCERR];
    }
    plc_log("error %d: %s", -errcode, errmsg);
}

config_t get_dio_values(const plc_t plc, 
                        const config_t state, 
                        BYTE type){
    config_t ret = state;
    sequence_t dios = get_sequence_entry(type, ret);
    if(dios == NULL || (
        type != CONFIG_DI &&
        type != CONFIG_DQ)) {
        
        return state;
    } 
    variable_t viter = dios->vars;
    int i = 0;
    BYTE val = 0;
    while(i < dios->size){
        if(viter != NULL) {
           
            if(type == CONFIG_DI){
                val = plc->di[i].I;
            } else if(type == CONFIG_DQ){
                val = plc->dq[i].Q;    
            }
            viter->params = update_param(
                                viter->params,
                                "STATE",
                                 val?"TRUE":"FALSE");	        
	        
	    }
	    viter = &(dios->vars)[++i];
    }
    return ret;
}

config_t get_aio_values(const plc_t plc, 
                        const config_t state, 
                        BYTE type){
    config_t ret = state;
    sequence_t aios = get_sequence_entry(type, ret);
    if(aios == NULL || (
        type != CONFIG_AI &&
        type != CONFIG_AQ &&
        type != CONFIG_MVAR)) {
        
        return state;
    } 
    variable_t viter = aios->vars;
    int i = 0;
    double val = 0;
    char valbuf[TINYBUF] = "";
    while(i < aios->size){
        if(viter != NULL) {
           
            if(type == CONFIG_AI){
                val = plc->ai[i].V;
            } else if(type == CONFIG_AQ){
                val = plc->aq[i].V;    
            } else if(type == CONFIG_MVAR){
                val = plc->mr[i].V;    
            }
            sprintf(valbuf, "%f", val);
            viter->params = update_param(
                                viter->params,
                                "VALUE",
                                 valbuf);	        
	        
	    }
	    viter = &(aios->vars)[++i];
    }
    return ret;
}


config_t get_reg_values(const plc_t plc, 
                        const config_t state){
    config_t ret = state;
    sequence_t regs = get_sequence_entry(CONFIG_MREG, ret);
    if(regs == NULL){
       
        return state;
    } 
    variable_t viter = regs->vars;
    int i = 0;
    uint64_t val = 0;
    while(i < regs->size){
        if(viter != NULL) {
           
            val = plc->m[i].V;    
            char vs[TINYBUF];
            memset(vs,0, TINYBUF);
            sprintf(vs,"%ld", val); 
            viter->params = update_param(
                                viter->params,
                                "VALUE",
                                 vs);	        
	    }
	    viter = &(regs->vars)[++i];
    }
    return ret;
}

config_t get_timer_values(const plc_t plc, 
                          const config_t state){
    config_t ret = state;
    sequence_t timers = get_sequence_entry(CONFIG_TIMER, ret);
    if(timers == NULL){
       
        return state;
    } 
    variable_t viter = timers->vars;
    int i = 0;
    long val = 0;
    BYTE out = 0;
    while(i < timers->size){
        if(viter != NULL) {
           
            val = plc->t[i].V;
            out = plc->t[i].Q;    
            char vs[TINYBUF];
            memset(vs,0, TINYBUF);
            sprintf(vs,"%ld",val); 
            viter->params = update_param(
                                viter->params,
                                "VALUE",
                                 vs);
            viter->params = update_param(
                                viter->params,
                                "OUT",
                                out?"TRUE":"FALSE");
                                             	        
	    }
	    viter = &(timers->vars)[++i];
    }
    return ret;
}

config_t get_pulse_values(const plc_t plc, 
                          const config_t state){
    config_t ret = state;
    sequence_t pulses = get_sequence_entry(CONFIG_PULSE, ret);
    if(pulses == NULL){
       
        return state;
    } 
    variable_t viter = pulses->vars;
    int i = 0;
    BYTE out = 0;
    while(i < pulses->size){
        if(viter != NULL) {
        
            out = plc->s[i].Q;    
            viter->params = update_param(
                                viter->params,
                                "OUT",
                                out?"TRUE":"FALSE");
                                             	        
	    }
	    viter = &(pulses->vars)[++i];
    }
    return ret;
}

config_t get_state(const plc_t plc, 
                   const config_t state){
    config_t r = state;
    int i = 0;
    //set status
    r = set_numeric_entry(0, plc->status, r);
    //assign values    
    if(plc->update & CHANGED_I){
        r = get_dio_values(plc, r, CONFIG_DI);
        //analog
        r = get_aio_values(plc, r, CONFIG_AI);
    }
    if(plc->update & CHANGED_O){
        r = get_dio_values(plc, r, CONFIG_DQ);
        //analog
        r = get_aio_values(plc, r, CONFIG_AQ);
    }
    if(plc->update & CHANGED_M){
    //registers
        r = get_reg_values(plc, r);
    //reals
        r = get_aio_values(plc, r, CONFIG_MVAR);
    }
    if(plc->update & CHANGED_T){
    //timers
        r = get_timer_values(plc, r);
    }
    if(plc->update & CHANGED_S){
    //pulses
        r = get_pulse_values(plc, r);
    }
    
    //show forced
    //add program
    if( plc->update & CHANGED_STATUS) {
        sequence_t programs = get_sequence_entry(CONFIG_PROGRAM, r);
    
        for(i = 0;i < plc->rungno; i++){
            codeline_t liter = plc->rungs[i]->code;
            int lineno = 0;
            char label[SMALLBUF] = "";
            while(liter){
                sprintf(label, "LINE %d", ++lineno);
            
                param_t code = get_param(label,programs->vars[i].params);
                if(code == NULL){ 
                    programs->vars[i].params = append_param(
                                        programs->vars[i].params,
                                        label,
                                        liter->line);
                }
                liter = liter->next;    
            }      
        }
    }
    return r;
}

plc_t apply_command(const config_t com, plc_t plc){
    char * confstr = "config.yml";
    char * cvalue = NULL;

           
    switch(get_numeric_entry(0, com)){
        case COM_START:
        
            plc = plc_start(plc);
            break;
            
        case COM_STOP:
        
            plc = plc_stop(plc);
            break;
        
        case COM_LOAD:
            plc = plc_stop(plc);
            Conf = init_config();
            cvalue = get_string_entry(1, com);
            if(cvalue == NULL ||
            cvalue[0] == 0){
                cvalue = confstr;
            }
            if ((load_config_yml(cvalue, Conf))->err < PLC_OK) {
                plc_log("Invalid configuration file %s\n", cvalue);
            } else {
                plc = init_emu(Conf);
            }    
            break;
            
        case COM_SAVE:
        
            plc = plc_stop(plc);
            cvalue = get_string_entry(1, com);
            if(cvalue == NULL){
                cvalue = confstr;
            }
            if ((save_config_yml(cvalue, Conf)) < PLC_OK) {
                plc_log("Invalid configuration file %s\n", cvalue);
            }
            break;    
        
        case COM_FORCE:
        case COM_UNFORCE:
        case COM_EDIT:        
        default: break;
    }
    return plc;
}
 
int main(int argc, char **argv)
{
    //int errcode = PLC_OK;
    int prog = 0;
    char * confstr = "config.yml";
    Conf = init_config();
    
    char * cvalue = NULL;
    opterr = 0;
    int c;
    while ((c = getopt (argc, argv, "hc:")) != PLC_ERR){
        switch (c) {
            case 'h':
                 plc_log(Usage);
                 return 1;
                break;
            case 'c':
                cvalue = optarg;
                break;
            case '?':
                plc_log(Usage);
                if (optopt == 'c'){
                    plc_log( 
                    "Option -%c requires an argument\n", optopt);
                } else if (isprint (optopt)){
                    plc_log(
                    "Unknown option `-%c'\n", optopt);
                } else{
                    plc_log(
                    "Unknown option character `\\x%x'\n",
                    optopt);
                }
                return 1;
            default:
               
                abort ();
        }
    }
    if(cvalue == NULL)
       cvalue = confstr;
       
    if ((load_config_yml(cvalue, Conf))->err < PLC_OK) {
        plc_log("Invalid configuration file %s\n", cvalue);
        return PLC_ERR;
    }
//initialize PLC
    Plc = init_emu(Conf);
    sequence_t programs = get_sequence_entry(CONFIG_PROGRAM, Conf);
    for(; programs && prog < programs->size; prog++){
        if(programs->vars[prog].name){
            Plc = plc_load_program_file(programs->vars[prog].name, Plc);
        }
    }
//start hardware
    enable_bus();
//start UI
    
    ui_init(Conf);
    //UiReady=more;
    config_t command = copy_sequences(Conf, ui_init_command());
    config_t state = copy_sequences(Conf, ui_init_state());
    Plc = plc_start(Plc);    
    while (get_numeric_entry(0, command)!=COM_QUIT) {
       if(Plc->update != 0){
           state = get_state(Plc, state);
           ui_draw(state);
           Plc->update = 0;
        }   
        command = ui_update(command);
        Plc = apply_command(command, Plc);
        Plc = plc_func(Plc);
    }
    sigkill();
    disable_bus();
    clear_config(Conf);
    close_log();
    ui_end();
    return 0;
}

