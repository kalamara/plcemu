#include <stdarg.h>
#include <string.h>
#include <stdio.h>


#include "config.h"
//#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

char * Mock_var  = NULL;
char * Mock_val  = NULL;
unsigned char Mock_op = 0xff;
int Mock_idx = -1;
char MsgStr[256];

void plc_log(const char * msg, ...)
{
    va_list arg;
    memset(MsgStr,0, 256);
    va_start(arg, msg);
    vsprintf(MsgStr,msg,arg);
    va_end(arg);
    printf("%s\n",MsgStr);
}

void ui_display_message(char *msgstr){
    printf("%s\n", msgstr);
}

plc_t declare_variable(const plc_t p, 
                        int var,
                        PLC_BYTE idx,                          
                        const char* val){
    Mock_op = var;
    Mock_idx = idx;                    
    Mock_val = val;                    
    return p;
}

plc_t configure_io_limit(const plc_t p, 
                        int io, 
                        PLC_BYTE idx, 
                        const char* val,
                        PLC_BYTE max){
    return p;
}

plc_t init_variable(const plc_t p, int var, PLC_BYTE idx, const char* val){
    return p;
}

plc_t configure_variable_readonly(const plc_t p, 
                                int var, 
                                PLC_BYTE idx, 
                                const char* val){
    return p;
}

plc_t configure_counter_direction(const plc_t p, 
                                    PLC_BYTE idx, 
                                    const char* val){
    return p;
}

plc_t configure_timer_scale(const plc_t p, 
                     PLC_BYTE idx, 
                     const char* val){
    return p;
}

plc_t configure_timer_preset(const plc_t p, 
                        PLC_BYTE idx, 
                        const char* val){
    return p;
}

plc_t configure_timer_delay_mode(const plc_t p, 
                        PLC_BYTE idx, 
                        const char* val){
    return p;
}

plc_t configure_pulse_scale(const plc_t p, 
                        PLC_BYTE idx, 
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

plc_t force(plc_t p, int op, PLC_BYTE i, char * val){
    Mock_val = val;
    Mock_op = op;
    Mock_idx = i;
    return p;
}

plc_t unforce(plc_t p, int op, PLC_BYTE i){
    Mock_val = NULL;
    if(Mock_op == op){
        Mock_op = 0xff;
        if(Mock_idx == i){
            Mock_idx  = -1;
            Mock_val  = NULL;
        }
    }
    return p;
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

void stub_dio_read(unsigned int n, PLC_BYTE* bit)
{	
}

void stub_dio_write(const unsigned char *buf, 
                    unsigned  int n, 
                    unsigned char bit)
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


/**
 * @brief entry point: load text file into configuration
 * @param filename (full path)
 * @param the configuration
 * @return new config
 */
config_t load_config(const char * filename, config_t conf){
    return NULL;
}


/**
 * @brief entry point: save configuration to text file 
 * @param filename (full path)
 * @param the configuration
 * @return OK or ERR
 */
int save_config(const char * filename, const config_t conf){
    return PLC_OK;
}

/**
 * @brief print configuration to a file
 * @param the open file
 * @param the configuration where the parsed values are stored
 * @return OK or ERR
 */        
int print_config(FILE * f, const config_t conf){
    return PLC_OK;    
}

/**
 * @brief serialize configuration to a string
 * @param the configuration where the parsed values are stored
 * @return string allocated with CONF_STR size  
 * must be consequently free'd.
 */          
char * serialize_config(const config_t conf){
    return NULL;
}

/**
 * @brief deserialize string to a config
 * @param a string
 * @param a config that defines the structure
 * @return the configuration where the parsed values are stored, or NULL
 */          
config_t deserialize_config(const char * buf, 
                            const config_t conf){
    return NULL;    
}
 
