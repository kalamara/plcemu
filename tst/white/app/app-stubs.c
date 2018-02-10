#include <stdarg.h>
#include <string.h>
#include <stdio.h>


#include "config.h"
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

void plc_log(const char * msg, ...)
{
    va_list arg;
    char msgstr[256];
    memset(msgstr,0, 256);
    va_start(arg, msg);
    vsprintf(msgstr,msg,arg);
    va_end(arg);
    printf("%s\n",msgstr);
}

void ui_display_message(char *msgstr){
    printf("%s\n", msgstr);
}

plc_t declare_variable(const plc_t p, 
                        int var,
                        BYTE idx,                          
                        const char* val){
    return p;
}

plc_t configure_io_limit(const plc_t p, 
                        int io, 
                        BYTE idx, 
                        const char* val,
                        BYTE max){
    return p;
}

plc_t init_variable(const plc_t p, int var, BYTE idx, const char* val){
    return p;
}

plc_t configure_variable_readonly(const plc_t p, 
                                int var, 
                                BYTE idx, 
                                const char* val){
    return p;
}

plc_t configure_counter_direction(const plc_t p, 
                                    BYTE idx, 
                                    const char* val){
    return p;
}

plc_t configure_timer_scale(const plc_t p, 
                     BYTE idx, 
                     const char* val){
    return p;
}

plc_t configure_timer_preset(const plc_t p, 
                        BYTE idx, 
                        const char* val){
    return p;
}

plc_t configure_timer_delay_mode(const plc_t p, 
                        BYTE idx, 
                        const char* val){
    return p;
}

plc_t configure_pulse_scale(const plc_t p, 
                        BYTE idx, 
                        const char* val){
    return p;
}

plc_t plc_start(plc_t p){
    return p;
}

plc_t plc_stop(plc_t p){
    return p;
}
                        
