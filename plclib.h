#ifndef _PLCLIB_H_
#define _PLCLIB_H_


/**PLC helper fuctions**/
#define FALSE 0
#define TRUE 1
//boolean function blocks supported
#define DI 0 	//digital input
#define DQ 1	//digital output
#define COUNTER 2	//pulse of counter
#define TIMER 	3 	//output of timer
#define BLINKER 4	//output of blinker

struct PLC_regs;


int PLC_task(struct PLC_regs*);
int PLC_init();
//internal functions-not to be used by user programs
void dec_inp(struct PLC_regs *);
void enc_out(struct PLC_regs * );
void write_mvars(struct PLC_regs * );
void read_mvars(struct PLC_regs * );

int contact(struct PLC_regs * , int , int ,unsigned char);
int resolve(struct PLC_regs * , int , int);

int re(struct PLC_regs * ,int ,int );
int fe(struct PLC_regs * ,int ,int );
int set(struct PLC_regs * , int , int );
int reset(struct PLC_regs * , int , int );

int down_timer(struct PLC_regs * , int );

#endif //_PLCLIB_H_
