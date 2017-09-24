#include <stdarg.h>
#include <time.h>

#include "plclib.h"
#include "ui.h"
#include "util.h"

extern int UiReady;
FILE * ErrLog = NULL;

void plc_log(const char * msg, ...) {
   va_list arg;
   time_t now;
   time(&now);
   char msgstr[MAXSTR];
   memset(msgstr,0,MAXSTR);
   va_start(arg, msg);
   vsprintf(msgstr,msg,arg);
   va_end(arg);
   if(!ErrLog)
      ErrLog = fopen(LOG,"w+");
   if(ErrLog){
      fprintf(ErrLog, "%s", msgstr);
      fprintf(ErrLog, ":%s", ctime(&now));
      fflush(ErrLog);
   }
   if(UiReady)
       ui_display_message(msgstr);
   else
       printf("%s\n",msgstr);
}

void close_log() {
  if(ErrLog)
    fclose(ErrLog);
}

/*******************debugging tools***************/

double Mean = 0;
double M2 = 0;
unsigned long Loop = 0;

void compute_variance(double x) {
    if(Loop == 0){//overflow
       Mean = 0;
       M2 = 0; 
    }    
    Loop++;    
    double delta = x - Mean;
    Mean += delta / (double)Loop;
    M2 += delta * (x - Mean);    
}

void get_variance(double * mean, double * var) {
    *mean = Mean;
    if(Loop > 1)
        *var = M2/(double)(Loop - 1);
}

unsigned long get_loop() {
    return Loop;
}

