#ifndef _PLCEMU_H_
#define _PLCEMU_H_
/**
 *@file plcemu.h
 *@brief main header
*/

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>
#ifndef ULTRALITE
    #include <curses.h>
#endif

#include "wedit.h"
#include "hardware.h"
#define VERSION 1.5//sys/io.h and other modernizations

enum
{
    PAGE_IO = 1,
    PAGE_EXIT,
    PAGE_EDIT,
    PAGE_FILE,
    PAGE_EDITMODE,
    PAGE_HELP,
    N_PAGES
}PAGES;

#define MAXBUF 65536
#define MAXSTR	1024
#define MEDSTR	256
#define SMALLSTR 128
#define SMALLBUF 64
#define TINYSTR 32
#define NICKLEN	16
#define BYTESIZE 8
#define LABELLEN 4

#define MILLION 1000000
#define THOUSAND 1000

#define ASCIISTART 0x30

#define TERMLEN 80
#define TERMHGT 24

#define KEY_TAB		9

enum
{
    N_ERR = -8, ///ERROR CODES are negative
    ERR_BADOPERATOR,
    ERR_BADCOIL,
    ERR_BADINDEX,
    ERR_BADOPERAND,
    ERR_BADFILE,
    ERR_BADCHAR,
    ERROR
}ERRORCODES;

enum
{
    LANG_LD,
    LANG_IL,
    LANG_ST
}LANGUAGES;

#define BYTE unsigned char
/**
 * @brief The digital_input struct
 */
struct digital_input
{
    BYTE I;///contact value
    BYTE RE;///rising edge
    BYTE FE;///falling edge
    char nick[NICKLEN];///nickname
};
/**
 * @brief The digital_output struct
 */
struct digital_output
{
    BYTE Q;//contact
    BYTE SET;//set
    BYTE RESET;//reset
    char nick[NICKLEN];//nickname
};
/**
 * @brief The timer struct.
 * struct which represents  a timer state at a given cycle
 */
struct timer
{
    long S;	///scale; S=1000=>increase every 1000 cycles. STEP= 10 msec=> increase every 10 sec
    long sn;///internal counter used for scaling
    long V;	///value
    BYTE Q;	///output
    long P;	///Preset value
    BYTE ONDELAY;///1=on delay, 0 = off delay
    BYTE START;///start command: must be on to count
    //BYTE RESET;///down command: sets V = 0
    char nick[NICKLEN];
};

/**
 * @brief The blink struct
 * struct which represents a blinker
 */
struct blink
{
    BYTE Q; ///output
    long S;	///scale; S=1000=>toggle every 1000 cycles. STEP= 10 msec=> toggle every 10 sec
    long sn;///internal counter for scaling
    char nick[NICKLEN];
};

/**
 * @brief The mvar struct
 * struct which represents a memory register / counter
 */
struct mvar
{
    long V;     ///Value
    BYTE RO;	///1 if read only;
    BYTE DOWN;	///1: can be used as a down counter
    BYTE PULSE;		///pulse for up/downcounting
    BYTE EDGE;		///edge of pulse
    BYTE SET;		///set pulse
    BYTE RESET;		///reset pulse
    char nick[NICKLEN];   ///nickname
};

/**
 * @brief The PLC_regs struct
 * The struct which contains all the software PLC registers
 */
struct PLC_regs
{
    BYTE *inputs;   ///input values
    BYTE *outputs;  ///output values
    BYTE *edgein;	///edges
    BYTE *maskin;	///masks used to force values
	BYTE *maskout;
	BYTE *maskin_N;
	BYTE *maskout_N;
    BYTE command;   ///serial command from plcpipe
    BYTE response;  ///response to named pipe
    BYTE status;    ///0 = stopped, 1= running
	struct digital_input * di;
	struct digital_output * dq;
	struct timer * t;
	struct blink * s;
	struct mvar * m;
};

struct PLC_regs plc;

int Step,Sigenable,Pagewidth,Nt,Ns,Nm,Di,Dq,Base,Wr_offs,Rd_offs,Comedi_file,Comedi_subdev_i,Comedi_subdev_q;
const char Pipe[MAXSTR];
const char Responsefile[MAXSTR];
const char Hw[MAXSTR];
const char com_nick[MEDSTR][NICKLEN];///comments for up to 256 serial commands

char Lines[MAXBUF][MAXSTR];///ladder lines
int Lineno;	///actual no of active lines
int Pos[MAXBUF];///cursor position in each line
BYTE Val[MAXBUF];///current resolved value of each line. NEW: if it is final, set to 3.
char Labels[MAXBUF][MAXSTR];

#endif //_PLCEMU_H_
