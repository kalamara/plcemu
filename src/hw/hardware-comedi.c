#include <string.h>
#include <comedilib.h>
#include "plcemu.h"
#include "util.h"
#include "plclib.h"
#include "config.h"
#include "hardware.h"

static comedi_t * it;
int Comedi_file;
int Comedi_subdev_i;
int Comedi_subdev_q;
int Comedi_subdev_ai;
int Comedi_subdev_aq;
extern plc_t Plc;

void hw_config(const config_t conf)
{
    config_t c = get_recursive_entry(CONFIG_SIM, conf);
    Comedi_file = get_numeric_entry(COMEDI_FILE, c);
    config_t sub = get_recursive_entry(COMEDI_SUBDEV, c);
    
    Comedi_subdev_i = get_numeric_entry(SUBDEV_IN, sub);
    Comedi_subdev_q = get_numeric_entry(SUBDEV_OUT, sub;
    Comedi_subdev_ai = get_numeric_entry(SUBDEV_ADC, sub;
    Comedi_subdev_aq = get_numeric_entry(SUBDEV_DAC, sub;
}

int enable_bus() /* Enable bus communication */
{
    int r = 0;

    char filestr[MEDSTR];
    memset(filestr, 0, MEDSTR);
    sprintf(filestr, "/dev/comedi%d", Comedi_file);
    printf("%s\n", filestr);
    if ((it = comedi_open(filestr)) == NULL )
        r = -1;
    //	printf("io card enabled\n");
    return r;
}

int disable_bus() /* Disable bus communication */
{
    int i, j, n;
    for (i = 0; i < Plc.nq; i++){
        //write zeros
        for (j = 0; j < BYTESIZE; j++){
            //zero n bit out
            n = BYTESIZE * i + j;
            dio_write(Plc.outputs, n, 0);
        }
    }
    comedi_close(it);
    return PLC_OK;
}


int io_fetch()
{//COMEDI has already fetched them for you
    return 0;
}

int io_flush()
{
    return 0;
}

void dio_read(unsigned int index, BYTE* value)
{	//write input n to bit
    unsigned int b;
    comedi_dio_read(it, Comedi_subdev_i, index, &b);
    *value = (BYTE) b;
}
void dio_write(const BYTE * value, int n, int bit)
{	//write bit to n output
    BYTE q;
    q = value[n / BYTESIZE];
    q |= bit << n % BYTESIZE;
    comedi_dio_write(it, Comedi_subdev_q, n, bit);
}

void dio_bitfield(const unsigned char *mask, unsigned char *bits)
{//simultaneusly write output bits defined my mask and read all inputs
    /*    FIXME int i;
    unsigned int w, b;
    w = (unsigned int) (*mask);
    b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}

void data_read(unsigned int index, uint64_t* value)
{
    lsampl_t data; 
    comedi_data_read(it,
 	    Comedi_subdev_ai,
 	    index,
 	    0,//unsigned int range,
 	    AREF_GROUND,//unsigned int aref,
 	    &data);
 	*value = (uint64_t)data;    
}
 	
void data_write(unsigned int index, uint64_t value)
{
    lsampl_t data = (lsampl_t)(value % 0x100000000); 
    comedi_data_write(it,
 	Comedi_subdev_aq,
 	index,
 	0,//unsigned int range,
 	AREF_GROUND,//unsigned int aref,
 	data);
}
 	
 	
 	
 	
 	
