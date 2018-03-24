#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_ 

#define LABELLEN 4
//#define NICKLEN	16
/**
 *IL modifiers
 */
typedef enum{
    IL_NEG = 1, /// '!'
    IL_PUSH,    /// '('
    IL_NORM,    /// ' '
    IL_COND,    ///'?'
    N_IL_MODIFIERS
}IL_MODIFIERS;

#define IS_MODIFIER(x) (x >= IL_NEG && x < N_IL_MODIFIERS) 

#define MAX_LABEL 1024

/**
 * @brief The instruction struct
 */
typedef struct instruction{
    char label[MAX_LABEL];
    char lookup[MAX_LABEL]; //label to lookup (applies to JMP etc.)
    unsigned char operation;
    unsigned char operand;
    unsigned char modifier;
    unsigned char byte;
    unsigned char bit;
} * instruction_t;

/**
 * @brief get type of instruction
 * @convention type is encoded in the instruction
 * the length is given by the bit part, and the scalar / real is defined by 
 * the operand (analog / digital)
 * @param instruction
 * @return the type, or error
 */
int get_type(const instruction_t ins);

/**
  *@brief deepcopy instructions
  *@param from
  *@param to
  */ 
void deepcopy(const instruction_t from, instruction_t to);

void dump_instruction( instruction_t ins, char * dump);

#endif // _INSTRUCTION_H_
