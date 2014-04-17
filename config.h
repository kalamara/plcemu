#ifndef _CONFIG_H_
#define _CONFIG_H_

#define STEP 2//cycle milliseconds
#define PIPE 	"/home/isobau/src/plcemu/plcpipe" //named pipe polled for commands
#define RESPONSE "/home/isobau/src/plcemu/plcresponse" //file /pipe for response
#define SIGENABLE 	36//POSIX enable signal
#define PAGELEN 64 //page max length
#define PAGEWIDTH	160
#define NT 8//number of timers
#define NS 8//number of blinking timers
#define NM 256//number of integer variables
//hardware
#define HW "Adlink PCI-1756"
#define BOOL_DI 4//number of bytes of digital inputs
#define DQ 4//number of bytes of digital outputs
//TODO analog io
//user space interface:
#define BASE 50176 //59392 //hw address base
#define WR_OFFS	4//write offset
#define RD_OFFS	0//read offset
//COMEDI interface:
#define COMEDI_FILE 0
#define COMEDI_SUBDEV_I 0
#define COMEDI_SUBDEV_Q 1

#endif //_ARGS_H_




































