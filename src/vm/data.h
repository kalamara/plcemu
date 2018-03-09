#ifndef _DATA_H_
#define _DATA_H_

#include <inttypes.h>

#define BYTE unsigned char
#define BIT(x) unsigned char x:1

#define NEGATE 128//negate second operand, not return value.
#define BOOLEAN   64
#define NOP	0

#define MAXBUF 256 
#define MAXSTR	1024
#define MEDSTR	256
#define SMALLSTR 128
#define SMALLBUF 64
#define TINYBUF 24
#define TINYSTR 32
#define COMMLEN 16

#define LONG_BYTES 8
#define LWORDSIZE 64
#define DWORDSIZE 32
#define WORDSIZE 16
#define BYTESIZE 8

/**
 *IL instructions
 */
typedef enum{
    ///IL OPCODES: 
    IL_NOP,///no operand
    IL_POP,     ///)
    IL_RET,         ///RET
    //arithmetic LABEL
    IL_JMP,         ///JMP
    //subroutine call (unimplemented)
    IL_CAL,         ///CAL
    //boolean, no modifier
    IL_SET,         ///S
    IL_RESET,       ///R
    //any operand, only negation
    IL_LD,          ///LD
    IL_ST,          ///ST
    //any operand, only push
    //boolean, all modifiers
    IL_AND,         ///AND
    IL_OR,          ///OR
    IL_XOR,         ///XOR
    IL_ADD,          ///ADD
    IL_SUB,          ///SUBTRACT
    IL_MUL,          ///MULTIPLY
    IL_DIV,          ///DIVIDE
    IL_GT,          ///GREATER THAN
    IL_GE,          ///GREATER OR EQUAL
    IL_EQ,          ///EQUAL
    IL_NE,          ///NOT EQUAL
    IL_LT,          ///LESS THAN
    IL_LE,          ///LESS OR EQUAL
    N_IL_INSN
}IL_INSN;

#define FALSE 0
#define TRUE 1

#define BOOL(x) x > 0 ? TRUE : FALSE 

#define FIRST_BITWISE IL_AND
#define FIRST_ARITHMETIC IL_ADD
#define FIRST_COMPARISON IL_GT

#define IS_BITWISE(x) (x >= FIRST_BITWISE && x < FIRST_ARITHMETIC)
#define IS_ARITHMETIC(x) (x >= FIRST_ARITHMETIC && x < FIRST_COMPARISON)
#define IS_COMPARISON(x) (x >= FIRST_COMPARISON && x < N_IL_INSN)
#define IS_OPERATION(x) (x >= FIRST_BITWISE && x < N_IL_INSN) 

typedef enum{
    T_BOOL, //- 1 bit
    T_BYTE, //- 8 bit (1 byte)
    T_WORD, //- 16 bit (2 byte)
    T_DWORD,//- 32 bit (4 byte)
    T_LWORD,//- 64 bit (8 byte)
    T_REAL,//- (8 byte) double floating point number   
    
    N_TYPES
}DATATYPES;

typedef enum{
///operands
    OP_INPUT = 20,  ///i
    OP_REAL_INPUT,  ///if 21
    OP_FALLING, 	///f  22
    OP_RISING,      ///r  23
    OP_MEMORY,  	///m  24
    OP_REAL_MEMORY, ///mf 25 
    OP_COMMAND,     ///c  26
    OP_BLINKOUT,    ///b  27
    OP_TIMEOUT,     ///t  28
    OP_OUTPUT,   	///q  29
    OP_REAL_OUTPUT, ///qf 30
    ///coils 
    OP_CONTACT,     ///Q  31
    OP_REAL_CONTACT,///QF 32
    OP_START,       ///T  33
    OP_PULSEIN,     ///M  34 
    OP_REAL_MEMIN,  ///MF 35
    OP_WRITE,       ///W  36
    OP_END,         ///0  37
    N_OPERANDS
}IL_OPERANDS;

#define OP_VALID(x) x >= OP_INPUT && x < N_OPERANDS
#define OP_REAL(x) x == OP_REAL_INPUT \
                || x == OP_REAL_MEMORY \
                || x == OP_REAL_OUTPUT \
                || x == OP_REAL_CONTACT \
                || x == OP_REAL_MEMIN


//TODO: add type for checkings and castings
typedef union accdata{
  uint64_t u; 
  double r;
} data_t; //what can the accumulator be

uint64_t operate_u(unsigned char op, uint64_t a, uint64_t b);

double operate_d(unsigned char op, double a, double b);

float operate_f(unsigned char op, float a, float b);

/**
 * @brief operate operator op of type t on data a and b
 * @param operator
 * @param type
 * @param a
 * @param b
 * @return result if available
 */
data_t operate( unsigned char op, 
                       unsigned char t, 
                       const data_t a, 
                       const data_t b);

#endif //_DATA_H_

