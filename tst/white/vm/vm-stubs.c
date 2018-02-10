#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

void plc_log(const char * msg, ...)
{
    va_list arg;
    char msgstr[MAXSTR];
    memset(msgstr,0,MAXSTR);
    va_start(arg, msg);
    vsprintf(msgstr,msg,arg);
    va_end(arg);
    printf("%s\n",msgstr);
}

int project_task(plc_t p)
{
    return 0;
}

int project_init()
{
    return 0;
}

void compute_variance( double x){}

int stub_config(const config_t conf)
{
        
    return PLC_OK;    
}

int stub_enable() /* Enable bus communication */
{
    int r = PLC_OK;
    
    return r;
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
    *bit = 0;
}

void stub_dio_write(const unsigned char *buf,  int n,  int bit)
{	//write bit to n output
}

void stub_dio_bitfield(const unsigned char* mask, unsigned char *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
}

void stub_data_read(unsigned int index, uint64_t* value)
{
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


