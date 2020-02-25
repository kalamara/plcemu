#ifndef _UTIL_H_
#define _UTIL_H_
/**logging, metrics, debugging stuff */

#define LOG "plcemu.log"
void plc_log(const char * msg, ...);
void close_log();
//char * strdup_r(char * dest, const char * src);
/*******************debugging tools****************/
void dump_label( char * label, char * dump);
void compute_variance( double x);
void get_variance( double * mean, double * var);
unsigned long get_loop();
#endif //_UTIL_H
