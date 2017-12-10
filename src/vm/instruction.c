#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "data.h"
#include "instruction.h"

const char IlCommands[N_IL_INSN][LABELLEN] = {
 		"",
		")",
		"RET",
		"JMP",
		"CAL",
		"S",
		"R",
		"LD",
		"ST",
		"AND",
		"OR",
		"XOR",
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

const char IlOperands[N_OPERANDS][3] = {
 		"i",
        "if",
		"f",
		"r",
		"m",
		"mf",
		"c",
		"b",
		"t",
		"q",
		"qf",
		"Q",
		"QF",
		"T",
		"M",
		"MF",
		"W",
		"",
};

const char IlModifiers[N_IL_MODIFIERS][2] = {
    "!",
    "(",
    " ",
    "?",
};

int get_type(const instruction_t ins)
{
    int rv = -1; //ERR
    
    if(ins != NULL 
    && OP_VALID(ins->operand)){
        unsigned char x = ins->bit;
        if(OP_REAL(ins->operand))
             rv = T_REAL;
        else{
            switch(x){
                case BYTESIZE:
                    rv = T_BYTE;
                    break;
                    
                case WORDSIZE:
                    rv = T_WORD;
                    break;
                    
                case DWORDSIZE:
                    rv = T_DWORD;
                    break;
                    
                case LWORDSIZE:
                    rv = T_LWORD;
                    break;
                    
                default:
                    if(0 <= x
                    && x < BYTESIZE)
                        rv = T_BOOL; 
            }
        }
    }
    return rv;
}

void deepcopy(const instruction_t from, instruction_t to)
{
    //deepcopy        
        to->operation = from->operation;
        to->operand = from->operand;
        to->modifier = from->modifier;
        to->byte = from->byte;
        to->bit = from->bit;
        if(from->label != NULL)
            strcpy(to->label, from->label);
         if(from->lookup != NULL)
            strcpy(to->lookup, from->lookup);
}

void dump_label(char * label, char * dump) {
    char buf[NICKLEN] = "";
    if(label[0] != 0)
        sprintf(buf, "%s:", label);
    strcat(dump, buf);
}

void dump_instruction(instruction_t ins, char * dump) {
    if(ins == NULL)
        return;
    char buf[8] = "";
    dump_label(ins->label, dump);
    strcat(dump, IlCommands[ins->operation]);
    if(ins->operation >= IL_RET){
        strcat(dump, IlModifiers[ins->modifier - 1]);
        if(ins->operation == IL_JMP){
            sprintf(buf, "%d", ins->operand);
            strcat(dump, buf);
        }
        else {
            strcat(dump, IlOperands[ins->operand - OP_INPUT]);    
            sprintf(buf, "%d/%d", ins->byte, ins->bit);
            strcat(dump, buf);
        }
    }
    strcat(dump, "\n");
}

