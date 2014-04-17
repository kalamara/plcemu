#ifndef _PLCLIB_H_
#define _PLCLIB_H_
/**
 *@file plclib.h
 *@brief PLC helper fuctions
*/

#define FALSE 0
#define TRUE 1

enum
{///boolean function blocks supported
    BOOL_DI,        ///digital input
    BOOL_DQ,        ///digital output
    BOOL_COUNTER,   ///pulse of counter
    BOOL_TIMER,     ///output of timer
    BOOL_BLINKER,   ///output of blinker
    N_BOOL
}BOOL_FB;

struct PLC_regs;

/**
 * @brief PLC task executed in a loop
 * @param pointer to PLC registers
 * @return OK or error code
 */
int PLC_task(struct PLC_regs* p);

/**
 * @brief PLC initialization executed once
 * @return
 */
int PLC_init();
/***internal functions-not to be used by user programs**/

/**
 * @brief decode inputs
 * @param pointer to PLC registers
 */
void dec_inp(struct PLC_regs * p);

/**
 * @brief encode outputs
 * @param pointer to PLC registers
 */
void enc_out(struct PLC_regs * p);

/**
 * @brief write values to memory variables
 * @param pointer to PLC registers
 */
void write_mvars(struct PLC_regs * p);

/**
 * @brief read_mvars
 * @param pointer to PLC registers
 */
void read_mvars(struct PLC_regs * p);

/***user interface***************************************/


/**
 * @brief rising edge of input
 * @param pointer to PLC registers
 * @param type is Digital Input Or Counter
 * @param index
 * @return true if rising edge of operand, false or error otherwise
 */
int re(const struct PLC_regs * p, int type, int idx);

/**
 * @brief falling edge of input
 * @param pointer to PLC registers
 * @param type is Digital Input Or Counter
 * @param index
 * @return true if falling edge of operand, false or error otherwise
 */
int fe(const struct PLC_regs * p, const int type, const int idx);

/**
 * @brief set output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @return OK if success or error code
 */
int set(struct PLC_regs * p, const int type, const int idx);

/**
 * @brief reset output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @return OK if success or error code
 */
int reset(struct PLC_regs * p, const int type,const int idx);

/**
 * @brief contact value to output
 * @param pointer to PLC registers
 * @param type is Digital Output, Timer or Counter
 * @param index
 * @param value
 * @return OK if success or error code
 */
int contact(struct PLC_regs * p, const int type, const int idx, const unsigned char val);

/**
 * @brief resolve an operand value
 * @param pointer to PLC registers
 * @param type of operand
 * @param index
 * @return return value or error code
 */
int resolve(struct PLC_regs * p, const int type, const int idx);

/**
 * @brief reset timer
 * @param pointer to PLC registers
 * @param index
 * @return OK if success or error code
 */
int down_timer(struct PLC_regs * p, const int idx);

#endif //_PLCLIB_H_
