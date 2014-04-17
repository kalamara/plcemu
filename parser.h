#ifndef _PARSER_H_
#define _PARSER_H_

/**
 *@file parser.h
 *@brief LD / IL parsing functions
*/

/**
 *accepted LD symbols: 0-9 for digits, and
 */
enum
{
    ///operators:
    LD_BLANK = 10,  ///blank character
    LD_COIL,        ///(
    LD_AND,         ///-
    LD_OR,          ///|
    LD_NOT,         ///!
    LD_NODE,        ///+
    LD_SET,         ///[ set
    LD_RESET,       ///] reset,
    LD_DOWN,    	///) negate coil
    ///operands
    LD_INPUT,      	///i
    LD_FALLING, 	///f
    LD_MEMORY,  	///m
    LD_COMMAND,     ///c
    LD_BLINKOUT,    ///b
    LD_RISING,      ///r
    LD_TIMEOUT,     ///t
    LD_OUTPUT,   	///q
    ///coils (work for IL too)
    LD_CONTACT,     ///Q
    LD_START,       ///T
    LD_PULSEIN,     ///M
    LD_WRITE,       ///W
    LD_END,         ///0
    N_LD_SYMBOLS
}LD_SYMBOLS;


/**
 *IL instructions
 */
enum
{
    ///IL OPCODES: no operand
    IL_POP = 1,     ///)
    IL_RET,         ///RET
    //arithmetic LABEL
    IL_JMP,         ///JMP
    //subroutine call (unimplemented)
    IL_CAL,         ///CAL
    //boolean, no modifier
    IL_SET,         ///S
    IL_RESET,       ///R
    //boolean, all modifiers
    IL_AND,         ///AND
    IL_OR,          ///OR
    IL_XOR,         ///XOR
    //any operand, only negation
    IL_LD,          ///LD
    IL_ST,          ///ST
    //any operand, only push
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

#define NEGATE 128//negate second operand, not return value.
#define BOOL   64

/**
 *IL modifiers
 */
enum
{
    IL_NEG = 1, /// '!'
    IL_PUSH,    /// '('
    IL_NORM,    /// ' '
    IL_COND,    ///'?'
    N_IL_MODIFIERS
}IL_MODIFIERS;

#define NOP	0
#define RESOLVED 	-1
#define FINAL	2

/**
 * @brief The opcode struct
 *AND, OR, XOR, ANDN, ORN, XORN.
 *TODO: byte type operations.
 *if op > 128 then value is negated first.
 */
struct opcode
{
    BYTE operation;
    BYTE value;
    struct opcode * next;
} * Stack;

/**
 * @brief The instruction struct
 */
struct instruction
{
    char label[MAXSTR];
    BYTE operation;
    BYTE operand;
    BYTE modifier;
    BYTE byte;
    BYTE bit;
};

/**
 * @brief read an integer from string
 * starting @ position start
 * @param line
 * @param start
 * @return number read or error
 */
int extract_number(const char * line, const unsigned int start);

/**
 * @brief read ONE character from line at index
 * and parse grammatically
 * @param line
 * @param c index
 * @return LD symbol
 */

int read_char(const char * line, const unsigned int c);

/**
 * @brief resolve LD operand
 * @param pointer to PLC registers
 * @param type
 * @param idx
 * @return resolved value
 */
int resolve_operand(struct PLC_regs *p, const int type, const int idx);

/**
 * @brief resolve coil contact
 * @param pointer to PLC registers
 * @param type
 * @param idx
 * @param val
 * @return resolved value
 */
int resolve_coil(struct PLC_regs * p, const int type, const int idx, const int val);

/**
 * @brief resolve coil set
 * @param pointer to PLC registers
 * @param type
 * @param idx
 * @param val
 * @return resolved value
 */
int resolve_set(struct PLC_regs * p, const int type,const  int idx,const  int val);

/**
 * @brief resolve coil reset
 * @param pointer to PLC registers
 * @param type
 * @param idx
 * @param val
 * @return resolved value
 */
int resolve_reset(struct PLC_regs * p,const  int type,const  int idx,const  int val);

/**
 * @brief parse horizontally each unresolved line i in Lines[i] up to '+','(',0.
 * @param pointer to PLC registers
 * @return OK or Error code
 */
int resolve_lines(struct PLC_regs * p);

/**
 * @brief for an array arr of integers ,return the smallest of indices i so that arr[i] =  min(arr) >= min
 * @param arr
 * @param min
 * @param max
 * @return the smallest of indices i
 */
int minmin(const int *arr, const int min, const int max);

/**
 * @brief LD main task
 * @param pointer to PLC registers
 * @return
 */
int LD_task(struct PLC_regs * p);

/**
 * @brief push an opcode and a value into stack.
 * @param op
 * @param val
 */
void push(const BYTE op, const BYTE val);

/**
 * @brief retrieve stack heads operation and operand, apply it to val and return result
 * @param val
 * @return result
 */
BYTE pop(BYTE val);

/**
 * @brief parse IL line
 * line format:[label:]<operator>[<modifier>[%<operand><byte>[/<bit>]]|<label>][;comment]
 * @param pc program counter
 * @return OK or error
 */
int parse_il_line(char * line, struct instruction * op, int pc);

/**
 * @brief execute IL instruction
 * @return OK or error
 */
int instruct(struct PLC_regs *p, struct instruction *op, int *pc);

/**
 * @brief operate operator on bytes a and b
 * @param op
 * @param a
 * @param b
 * @return result if available
 */
BYTE operate(BYTE op, BYTE a, BYTE b);

#endif //_PARSER_H
