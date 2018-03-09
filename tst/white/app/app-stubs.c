#include <stdarg.h>
#include <string.h>
#include <stdio.h>


#include "config.h"
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

char * Mock_force_val  = NULL;
unsigned char Mock_force_op = 0;
int Mock_force_idx = 0;

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
    p->status = ST_RUNNING;
    return p;
}

plc_t plc_stop(plc_t p){
    p->status = ST_STOPPED;
    return p;
}

plc_t force(plc_t p, int op, BYTE i, char * val){
    Mock_force_val = val;
    Mock_force_op = op;
    Mock_force_idx = i;
    return p;
}

plc_t unforce(plc_t p, int op, BYTE i){
    Mock_force_val = NULL;
    Mock_force_op = op;
    Mock_force_idx = i;

}

struct PLC_regs Mock_plc;
              
plc_t new_plc(
    int di, 
    int dq,
    int ai,
    int aq,
    int nt, 
    int ns,
    int nm,
    int nr,
    int step,
    hardware_t hw){

    return &Mock_plc;
}

void clear_plc(plc_t plc){
}
                            
//stub hardware                        
int stub_config(const config_t conf)
{
    return PLC_OK;    
}

int stub_enable() /* Enable bus communication */
{    
    return PLC_OK;
}

int stub_disable() /* Disable bus communication */
{
    return PLC_OK;
}

int stub_fetch()
{
    return PLC_OK;
}

int stub_flush()
{
    return PLC_OK;
}

void stub_dio_read(unsigned int n, BYTE* bit)
{	
}

void stub_dio_write(const unsigned char *buf, unsigned  int n, unsigned char bit)
{	//write bit to n output
}

void stub_dio_bitfield(const unsigned char* mask, unsigned char *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
}

void stub_data_read(unsigned int index, uint64_t* value)
{
    *value = 0;
}

void stub_data_write(unsigned int index, uint64_t value)
{
}

struct hardware Hw_stub = {
    HW_SIM,
    0, //errorcode
    "stubbed hardware",
    stub_enable,// enable
    stub_disable, //disable
    stub_fetch, //fetch
    stub_flush, //flush
    stub_dio_read, //dio_read
    stub_dio_write, //dio_write
    stub_dio_bitfield, //dio_bitfield
    stub_data_read, //data_read
    stub_data_write, //data_write
    stub_config, //hw_config
};                       

hardware_t get_hardware( int type){
    return &Hw_stub;    
}                        
 
