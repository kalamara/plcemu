#include <ctype.h>
#include "plcemu.h"
#include "plclib.h"
#include "parser.h"

extern BYTE Command, Response;

const char IlCommands[N_IL_INSN][LABELLEN] =
{ 		"",
		")",
		"RET",
		"JMP",
		"CAL",
		"S",
		"R",
		"AND",
		"OR",
		"XOR",
		"LD",
		"ST",
		"ADD",
		"SUB",
		"MUL",
		"DIV",
		"GT",
		"GE",
		"EQ",
		"NE",
		"LT",
		"LE"
};

//TODO: IL byte operations

extern int Di, Dq, Nm, Nt, Ns;

int IL_task(struct PLC_regs * p)
{
	struct instruction op;
    int l, j, r = 0;
	for (j = 0; j < Lineno; j++)
	{
		r = parse_il_line(Lines[j], &op, j);
		if (r < 0)
		{
			Lines[j][0] = '.';
			break;
		}
		l = j;
		r = instruct(p, &op, &l);
		j = l;
		if (r < 0)
		{
			Lines[j][0] = '.';
			break;
		}
	}
	return r;
}

/*********************************STACK****************************************/

void push(const BYTE op,const BYTE val)
{//push an opcode and a value into stack.
	struct opcode * p;
	//malloc a struct opcode pointer
	p = malloc(sizeof(struct opcode));
	//initialize
	p->operation = op;
	p->value = val;
	p->next = Stack;
	//set stack head pointer to point at it
	Stack = p;
}

BYTE pop(BYTE val)
{//retrieve stack heads operation and operand, apply it to val and return result
	BYTE r = val; //return value
	struct opcode *p;
	if (Stack != NULL) //safety
	{
		r = operate(Stack->operation, Stack->value, val); //execute instruction
		p = Stack;
		Stack = Stack->next; //set stack head to point to next opcode in stack
		free(p); //free previous head.
	}
	return r;
}

/***************************INSTRUCTION LIST***********************************/
int parse_il_line(char * line, struct instruction * op, int pc)
{ //    line format:[label:]<operator>[<modifier>[%<operand><byte>[/<bit>]]|<label>][;comment]
	char buf[MAXSTR];
//	char tst[Pagewidth];
    int i=0;
    int byte=0;
    int bit=0;
	char * str;
	char * cursor;
	char op_buf[LABELLEN];
	char label_buf[MAXSTR];
	unsigned int idx = 0;
    BYTE modifier=0;
    BYTE operand=0;
    BYTE operator=0;

	memset(label_buf, 0, MAXSTR);
	memset(buf, 0, MAXSTR);
	//1. read up to ';' or /n
	while (line[idx] != 0 && line[idx] != '\n' && line[idx] != ';')
		idx++;

	for (i = idx; i < MAXSTR; i++)
		line[i] = 0; //trunc comments

	idx = 0;
	//2. find last ':', trunc up to there, store label.
	str = strrchr(line, ':');
	cursor = line;
	i = 0;
	if (str)
	{
		while (line + i + 1 != str)
			label_buf[i++] = line[i + 1];
        strcpy(Labels[pc], label_buf);
		strcpy(buf, str + 1);
//printf("label:%s\n",label_buf);
	}
	else
		strcpy(buf, line + 1);
	//3. find first ' ','!','('. store modifier (0 if not found)
	str = strchr(buf, '(');
	if (str) //push stack
		modifier = IL_PUSH;
	else
	{ //negate
		str = strchr(buf, '!');
		if (str)
			modifier = IL_NEG;
		else
		{ //normal
			str = strchr(buf, ' ');
			if (str)
				modifier = IL_NORM;
			else
			{ //conditional branch
				str = strchr(buf, '?');
				if (str)
					modifier = IL_COND;
				else
					modifier = NOP;
			}
		}
	}
//printf("modifier:%d\n",modifier);
	//4. read operator from beginning to modifier. check if invalid, (return error)
	if (str)
	{
		cursor = buf;
		i = 0;
		memset(op_buf, 0, LABELLEN);
		while (cursor + i != str && i < LABELLEN)
			op_buf[i++] = cursor[i];
	}
	else
	{
		if (strlen(buf) < LABELLEN)
			strcpy(op_buf, buf);
		else
			return ERR_BADOPERATOR;
	}
//printf("operator:%s\n",op_buf);        	
	operator = N_IL_INSN;
	for (i = 0; i < N_IL_INSN; ++i)
	{
		if (!strcmp(op_buf, IlCommands[i]))
			operator = i;
	}
//printf("(%d)\n",operator);
	if (operator == N_IL_INSN)
		return ERR_BADOPERATOR;

	if (operator > IL_CAL)
	{ //5. if operator supports arguments, find first '%'. (special case: JMP (string). if not found return error

		str = strchr(buf, '%');
		if (!str)
			return ERR_BADCHAR;
		operand = 0;
		//6.  read first non-numeric char after '%'. if not found return error. store operand. chack if invalid (return error).
		if (isalpha(str[1]))
			operand = read_char(str, 1);
		else
			return ERR_BADOPERAND;
//printf("operand:%d\n",operand);
		//7.  read first numeric chars after operand. store byte. if not found or out of range, return error.
		byte = extract_number(str, 2);
		if (byte < 0)
			return ERR_BADINDEX;
//printf("byte:%d\n",byte);	
		//BYTESIZE.  find '/'. if not found truncate, return.
		cursor = strchr(str, '/');
		bit = BOOL;
		if (cursor)
		{
			if (!isdigit(cursor[1]) || cursor[1] > '7')
				return ERR_BADINDEX;
			else
				bit = cursor[1] - '0';
//printf("bit:%d\n",bit);
		}
		//9.  if found, read next char. if not found, or not numeric, or >7, return error. store bit.
	}    // otherwise truncate and return
	else if (operator == IL_JMP)
	{
		str = strchr(buf, ' ');
		if (!str)
			return ERR_BADOPERAND;
		strcpy(op->label, str + 1);
	}
	op->operation = operator;
	op->modifier = modifier;
	op->operand = operand;
	op->byte = byte;
	op->bit = bit;
	return pc++;
}

/*IL_task
 parsing:
 2. switch operator:
 valid ones:
 )	pop
 only boolean:(bitwise if operand is byte)

 S 	set
 R 	reset

 AND
 OR
 XOR
 any:
 LD 	load
 ST 	store
 ADD
 SUB
 MUL
 DIV
 GT	>
 GE	>=
 NE	<>
 EQ	==
 LE	<=
 LT	<
 JMP
 unimplemented:
 CAL
 RET

 3. switch modifier
 ) cannot have one
 S, R can only have ' '
 LD, ST cannot have (
 AND, OR, XOR can have all 3
 the rest can only have '('
 if '!' set negate
 if '(' push stack
 4. switch operand
 valid ones:
 BOOL_DI	digital input
 R	rising edge
 F	falling edge
 DQ	digital output
 MH	memory high byte
 ML	memory low byte
 MP	pulse byte: 0000-SET-RESET-EDGE-VALUE
 B	blinker
 TQ	timer output
 TI	timer start
 W	serial output
 C	serial input
 5. resolve operand byte: 0 to MAX
 6. resolve operand bit: 0 to BYTESIZE
 7. execute command if no errors
 */

int instruct(struct PLC_regs *p, struct instruction *op, int *pc)
{
	static BYTE acc;    //accumulator
    BYTE boolflag = 0;
    //char tst[Pagewidth];
	int i;
//modifier check
	if (op->operation > IL_XOR && op->operation < IL_ADD
			&& op->modifier != IL_NEG && op->modifier != IL_NORM) //only negation
		return ERR_BADOPERATOR;

	if (op->operation > IL_ST    // only push
	&& op->modifier != IL_PUSH && op->modifier != IL_NORM)
		return ERR_BADOPERATOR;

	if (op->operation > IL_CAL && op->operation < IL_AND
			&& op->modifier != IL_NORM)    //no modifier
		return ERR_BADOPERATOR;

	if (op->bit < BYTESIZE)
		boolflag = 1;

//operand check
	if (op->operation == IL_SET || op->operation == IL_RESET
			|| op->operation == IL_ST)
	{
        if (op->operand < LD_CONTACT)
		{
            if (op->operand == LD_OUTPUT)
                op->operand = LD_CONTACT;
            else if (op->operand == LD_MEMORY)
                op->operand = LD_PULSEIN;
            else if (op->operand == LD_TIMEOUT)
                op->operand = LD_START;
			else
				return ERR_BADOPERAND;    //outputs
		}
	}
	else if (op->operation > IL_CAL
            && (op->operand < LD_INPUT || op->operand > LD_CONTACT))
		return ERR_BADOPERAND;    //inputs

	switch (op->operation)
	{
//IL OPCODES: no operand
	case IL_POP: //POP
		acc = pop(acc);
		break;
	case IL_RET:		//RET
//unimplemented yet: retrieve  previeous program , counter, set pc
		break;
//arithmetic LABEL
	case IL_JMP:			//JMP
		for (i = 0; i < Lineno; i++)
		{
			if (isalnum(Labels[i][0]))
			{
				if (!strcmp(op->label, Labels[i]))
				{
//		    sprintf(tst, "Line: %d, Label: %s go from %d to %d",i,Labels[i],*pc, i);
//	    	    draw_info_line(4+Pagelen/2,tst);
                    *pc = i;
				}
			}
		}
//retrieve line number from label, set pc
		break;
//subroutine call (unimplemented)
	case IL_CAL:			//retrieve subroutine line, set pc

		break;
//boolflagean, no modifier, outputs.
	case IL_SET:	//S
		switch (op->operand)
		{
        case LD_CONTACT:	//set output %QX.Y
			if (!boolflag)	//only gets called when bit is defined
				return ERR_BADOPERAND;
            set(p, BOOL_DQ, (op->byte) / BYTESIZE + op->bit);
			break;
        case LD_START:	//bits are irrelevant
            set(p, BOOL_TIMER, op->byte);
			break;
        case LD_PULSEIN:	//same here
            set(p, BOOL_COUNTER, op->byte);
			break;
		default:
			return ERR_BADOPERAND;
		}
		break;
	case IL_RESET:	//R
		switch (op->operand)
		{
        case LD_CONTACT:	//set output %QX.Y
			if (!boolflag)	//only gets called when bit is defined
				return ERR_BADOPERAND;
            reset(p, BOOL_DQ, (op->byte) / BYTESIZE + op->bit);
			break;
        case LD_START:	//bits are irrelevant
            reset(p, BOOL_TIMER, op->byte);
			break;
        case LD_PULSEIN:	//same here
            reset(p, BOOL_COUNTER, op->byte);
			break;
		default:
			return ERR_BADOPERAND;
		}
		break;
	case IL_LD:	//LD
		switch (op->operand)
		{
        case LD_OUTPUT:	//set output %QX.Y
			if (!boolflag)	//word
				acc = p->outputs[op->byte];
			else
                acc = resolve(p, BOOL_DQ, (op->byte) / BYTESIZE + op->bit);
			break;
        case LD_INPUT:	//load input %IX.Y
			if (!boolflag)	//word
				acc = p->inputs[op->byte];
			else
				acc = resolve(p, BOOL_DI, (op->byte) / BYTESIZE + op->bit);
			break;
        case LD_MEMORY:
			if (!boolflag)	//word
				acc = p->m[op->byte].V;
			else
                acc = resolve(p, BOOL_COUNTER, op->byte);
			break;
        case LD_TIMEOUT:
			if (!boolflag)//a convention: bit is irrelevant, but defining it means we are referring to t.Q, otherwise t.V
				acc = p->t[op->byte].V;
			else
                acc = resolve(p, BOOL_TIMER, op->byte);
			break;
        case LD_BLINKOUT:	//bit is irrelevant
            acc = resolve(p, BOOL_BLINKER, op->byte);
			break;
        case LD_COMMAND:
			acc = p->command;
			break;
        case LD_RISING:	//only boolean
			if (!boolflag)	//only gets called when bit is defined
				return ERR_BADOPERAND;
			acc = re(p, BOOL_DI, (op->byte) / BYTESIZE + op->bit);
			break;
        case LD_FALLING:	//only boolflagean
			if (!boolflag)	//only gets called when bit is defined
				return ERR_BADOPERAND;
			acc = fe(p, BOOL_DI, (op->byte) / BYTESIZE + op->bit);
			break;
		default:
			return ERR_BADOPERAND;
			break;
		}
		//if negate, negate acc
		if (op->modifier == IL_NEG)
			acc = 255 - acc;

		break;
	case IL_ST:	//ST: output
		//if negate, negate acc
		if (op->modifier == IL_NEG)
			acc = 255 - acc;
		switch (op->operand)
		{
        case LD_CONTACT:	    //set output %QX.Y
			if (!boolflag)	    //word
				p->outputs[op->byte] = acc;
			else
                contact(p, BOOL_DQ, (op->byte) / BYTESIZE + op->bit, acc % 2);
			break;
        case LD_START:	    //bits are irrelevant
            contact(p, BOOL_TIMER, op->byte, acc % 2);
			break;
        case LD_PULSEIN:
			if (!boolflag)	    //word
				p->m[op->byte].V = acc;
			else
                contact(p, BOOL_COUNTER, op->byte, acc % 2);
			break;
        case LD_WRITE:
			p->command = acc;
		default:
			return ERR_BADOPERAND;
		}

//any operand, only push
		break;
	default:	    //all others
		if (op->modifier == IL_NEG)
			op->operand += NEGATE;

		if (op->modifier == IL_PUSH)
		{
			push(op->operation, acc);
			op->operation = IL_LD;
			op->modifier = IL_NORM;
            instruct(p, op, pc);	    //recursiveness?
		}
		else
			acc = operate(op->operation, acc, op->operand);
		break;
	}
//        sprintf(tst, "Lines: %d, Operation: %d Operand:%d Mod:%d Accumulator:%d",Lineno, op->operation,op->operand,op->modifier,acc);
//	draw_info_line(4+Pagelen/2,tst);
	return acc;
}

BYTE operate(BYTE op, BYTE a, BYTE b)
{
	BYTE modularize, r = 0;	    //return value
	modularize = 0;
	if (op & BOOL)
	{	    //BOOLean type instruction
		a &= TRUE;	    //consider only LSB

		b = (b > 0) ? TRUE : FALSE;	    //consider only LSB
		op -= BOOL;
		modularize = TRUE;
	}
	if (op & NEGATE)
	{	    //negate b
		op -= NEGATE;
		b = 255 - b;
	}
	switch (op)
	{
	//boolean or bitwise, all modifiers,
	case IL_AND:	//AND
		r = a & b;
		break;

	case IL_OR:	//OR
		r = a | b;
		break;

	case IL_XOR:	//XOR
		r = a ^ b;
		break;
	case IL_ADD:
		if (a + b > 255)
			r = a + b - 255;
		else
			r = a + b;
		break;
	case IL_SUB:
		if (a >= b)
			r = a - b;
		else
			r = MEDSTR - b + a;	//2's complement
		break;
	case IL_MUL:
		r = (a * b) % MEDSTR;
		break;
	case IL_DIV:
		r = a / b;
		break;
	case IL_GT:
		r = (a > b);
		break;
	case IL_GE:
		r = (a >= b);
		break;
	case IL_EQ:
		r = (a == b);
		break;
	case IL_NE:
		r = (a != b);
		break;
	case IL_LT:
		r = (a < b);
		break;
	case IL_LE:
		r = (a <= b);
		break;
	default:
//		return ERR_BADOPERAND;
		break;
	}
//    }    
	if (modularize)
		return r % 2;
	else
		return r;
}

