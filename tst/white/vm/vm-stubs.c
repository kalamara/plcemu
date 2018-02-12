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
/********************stubbed hardware****************/

unsigned char Mock_din = 0;
unsigned char Mock_dout = 0;
int Mock_flush_count = 0;
uint64_t Mock_ain = 0;
uint64_t Mock_aout = 0;
int stub_config(const config_t conf)
{
    return PLC_OK;    
}

int stub_enable() /* Enable bus communication */
{
    Mock_din = 0;
    Mock_ain = 0;
           
    int r = PLC_OK;
    
    return r;
}

int stub_disable() /* Disable bus communication */
{
    Mock_aout = 0;
    Mock_dout = 0;
    Mock_flush_count = 0;

    return PLC_OK;
}

int stub_fetch()
{
    Mock_din = 1;
    Mock_ain = 0xABCDEF01;
    return PLC_OK;
}

int stub_flush()
{
    Mock_flush_count = 1;    
    return PLC_OK;
}

void stub_dio_read(unsigned int n, BYTE* bit)
{	
    *bit = Mock_din;
}

void stub_dio_write(const unsigned char *buf, unsigned  int n, unsigned char bit)
{	//write bit to n output
    if(n < 8)
        Mock_dout += (bit << n);
}

void stub_dio_bitfield(const unsigned char* mask, unsigned char *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
}

void stub_data_read(unsigned int index, uint64_t* value)
{
    *value = Mock_ain;
}

void stub_data_write(unsigned int index, uint64_t value)
{
    Mock_aout = value;
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


