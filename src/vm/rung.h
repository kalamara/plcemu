#ifndef _RUNG_H_
#define _RUNG_H_

#define MAXSTACK 256
#define PLC_OK 0
#define PLC_ERR -1

/**
 * @brief The opcode struct
 *AND, OR, XOR, ANDN, ORN, XORN.
 *TODO: byte type operations.
 *if op > 128 then value is negated first.
 */
typedef struct opcode{
    BYTE operation;
    BYTE type;
    BYTE depth;
    union accdata value;
    struct opcode * next;
} * opcode_t;

/**
 * @brief The instruction list executable rung
 */
typedef struct rung{
  instruction_t * instructions;
  char * code; ///original code for visual representation
  unsigned int insno;///actual no of active lines
  struct rung * next; ///linked list of rungs
  opcode_t stack; ///head of stack
  struct opcode prealloc[MAXSTACK]; ///preallocated stack    
  union accdata acc;    ///accumulator
} * rung_t;

/**
 * @brief take the next available member in the preallocated stack
 * @param the rung where the stack belongs
 * @return the candidate stack head
 */
opcode_t take(rung_t r);

/**
 * @brief give the stack head back to the stack
 * @param the head to give
 */
void give( opcode_t head);

/**
 * @brief push an opcode and a value into rung's stack.
 * @param op the operation
 * @param t the type
 * @param val
 * @param the rung //pointer to head of stack
 * @return OK or error
 */
int push( BYTE op, 
          BYTE t, 
          const data_t val,
          rung_t r );
         
/**
 * @brief retrieve stack heads operation and operand,
 * apply it to val and return result
 * @param val
 * @param pointer to head of stack
 * @return result
 */
data_t pop( const data_t val, opcode_t *stack);

/**
 * @brief get instruction reference from rung
 * @param r a rung AKA instructions list
 * @param i the instruction reference
 * @param idx the index
 * @return OK or error
 */
int get( const rung_t r, 
         unsigned int idx, 
         instruction_t * i);

/**
 * @brief append instruction to rung
 * @param i a new instruction
 * @param r a rung AKA instructions list
 * @return OK or error
 */
int append( const instruction_t i, rung_t r);

/**
 * @brief clear rung from instructions and free memory
 * @param r a rung AKA instructions list
 */
void clear_rung( rung_t r);

/**
 * @brief lookup instruction by label
 * @param label
 * @param r a rung AKA instructions list
 * @return the index (pc) of the instruction, or error if not found
 */
int lookup( const char * label, rung_t r);

/**
 * @brief intern  labels
 * for each lookup command (JMP etc.) the lookup label is 
 * looked up and if found the resulting index stored to the modifier
 * @param r a rung AKA instructions list
 * @return OK, or error if : a label is not found or found duplicate
 */
int intern( rung_t r);

void dump_rung( rung_t ins, char * dump);

#endif //_RUNG_H_
