#include "plcemu.h"
#include "hardware.h"
#include <comedilib.h>

static comedi_t * it;

int enable_bus() /* Enable bus communication */
{
	int uid;
	int r = 0;

    char filestr[MEDSTR];
	memset(filestr, 0, MEDSTR);
	sprintf(filestr, "/dev/comedi%d", Comedi_file);
	printf(filestr);
	if ((it = comedi_open(filestr)) == NULL )
		r = -1;
//	printf("io card enabled\n");
	return r;
}

int disable_bus() /* Disable bus communication */
{
    int uid, i, j, n;//, r = 0;
	for (i = 0; i < Dq; i++)
	{	//write zeros
		for (j = 0; j < BYTESIZE; j++)
		{	//zero n bit out
			n = BYTESIZE * i + j;
			dio_write(plc.outputs, n, 0);
		}
	}
	comedi_close(it);
    return 1;
}


int dio_fetch(long timeout)
{
    int bytesRead = 0;
    return bytesRead;
}

int dio_flush()
{
    int bytesWrote = 0;
    return bytesWrote;
}

void dio_read(const int index, BYTE* value)
{	//write input n to bit
	unsigned int b;
    comedi_dio_read(it, Comedi_subdev_i, index, &b);
    *value = (BYTE) b;
}
void dio_write(const BYTE * value,const int n,const int bit)
{	//write bit to n output
	BYTE q;
    q = value[n / BYTESIZE];
	q |= bit << n % BYTESIZE;
	comedi_dio_write(it, Comedi_subdev_q, n, bit);
}

void dio_bitfield(const unsigned char *mask, unsigned char *bits)
{	//simultaneusly write output bits defined my mask and read all inputs
/*    FIXME int i;
    unsigned int w, b;
    w = (unsigned int) (*mask);
	b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}
