#include <string.h>

#include "util.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "config.h"
#include "hardware.h"
#include "schema.h"

struct hardware Comedi;

#ifdef COMEDI
#include <comedilib.h>

static comedi_t * it;
int Comedi_file;
int Comedi_subdev_i;
int Comedi_subdev_q;
int Comedi_subdev_ai;
int Comedi_subdev_aq;

int com_config(const config_t conf)
{
    config_t c = get_recursive_entry(CONFIG_COMEDI, conf);
    Comedi_file = get_numeric_entry(COMEDI_FILE, c);
    config_t sub = get_recursive_entry(COMEDI_SUBDEV, c);
    
    Comedi_subdev_i = get_numeric_entry(SUBDEV_IN, sub);
    Comedi_subdev_q = get_numeric_entry(SUBDEV_OUT, sub);
    Comedi_subdev_ai = get_numeric_entry(SUBDEV_ADC, sub);
    Comedi_subdev_aq = get_numeric_entry(SUBDEV_DAC, sub);
    
    Comedi.label = get_string_entry(CONFIG_HW, conf);
    
     if(Comedi_file >= 0) 
         
        return PLC_OK;
    else 
        return PLC_ERR;
    
    return 0;
}

int com_enable() /* Enable bus communication */
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

int com_disable() /* Disable bus communication */
{
    comedi_close(it);
    return PLC_OK;
}


int com_fetch()
{//COMEDI has already fetched them for you
    return 0;
}

int com_flush()
{
    return 0;
}

void com_dio_read(unsigned int index, BYTE* value)
{	//write input n to bit
    unsigned int b;
    comedi_dio_read(it, Comedi_subdev_i, index, &b);
    *value = (BYTE) b;
}
void com_dio_write(const BYTE * value, unsigned int n, unsigned char bit)
{	//write bit to n output
    comedi_dio_write(it, Comedi_subdev_q, n, bit);
}

void com_dio_bitfield(const unsigned char *mask, unsigned char *bits)
{//simultaneusly write output bits defined my mask and read all inputs
    /*    FIXME int i;
    unsigned int w, b;
    w = (unsigned int) (*mask);
    b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}

void com_data_read(unsigned int index, uint64_t* value)
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
 	
void com_data_write(unsigned int index, uint64_t value)
{
    lsampl_t data = (lsampl_t)(value % 0x100000000); 
    comedi_data_write(it,
 	Comedi_subdev_aq,
 	index,
 	0,//unsigned int range,
 	AREF_GROUND,//unsigned int aref,
 	data);
}
 
struct hardware Comedi = {
    HW_COMEDI,
    0, //errorcode
    "Comedi driver",
    com_enable,// enable
    com_disable, //disable
    com_fetch, //fetch
    com_flush, //flush
    com_dio_read, //dio_read
    com_dio_write, //dio_write
    com_dio_bitfield, //dio_bitfield
    com_data_read, //data_read
    com_data_write, //data_write
    com_config, //hw_config
}; 	

#else 

struct hardware Comedi;
    
#endif //COMEDI 	
 	
 	
