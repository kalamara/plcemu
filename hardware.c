#include "plcemu.h"
#include "hardware.h"
//todo COMEDI probing
#ifdef USE_COMEDI
static comedi_t * it;
#endif
int enable_bus() /* Enable bus communication */
{
	int uid;
	int r = 0;
#ifndef USE_COMEDI
	uid=getuid(); /* get User id */
	seteuid(0); /* set User Id to root (0) */
	if (geteuid()!=0)
	{
		fprintf(stderr,"FATAL ERROR: UNABLE TO CHANGE TO ROOT\n");
		r = -1;
	}
	if (iopl(3)) /* request bus WR i/o permission */
	{
		fprintf(stderr,"FATAL ERROR: UNABLE TO GET I/O PORT PERMISSION\n");
		perror("iopl() ");
		seteuid(uid);
		r = -1;
	}
	seteuid(uid); /* reset User Id */
	outb(0,Base+Wr_offs); //clear outputs port
#else
    char filestr[MEDSTR];
	memset(filestr, 0, MEDSTR);
	sprintf(filestr, "/dev/comedi%d", Comedi_file);
	printf(filestr);
	if ((it = comedi_open(filestr)) == NULL )
		r = -1;
#endif
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
#ifndef USE_COMEDI
	uid=getuid(); /* get User id */
	setuid(0); /* set User Id to root (0) */
	if (getuid()!=0)
	{
		fprintf(stderr,"Unable to change id to root\nExiting\n");
		return -1;
	}
	if (iopl(0)) /* Normal i/o prevelege level */
	{
		perror("iopl() ");
		setuid(uid);
		return -1;
	}
	setuid(uid); /* reset User Id */
#else
	comedi_close(it);
#endif
    return 0;
}

void dio_read(const int index, BYTE* value)
{	//write input n to bit
	unsigned int b;
#ifndef USE_COMEDI
	BYTE i;
    i = inb(Base+Rd_offs+index/BYTESIZE);
    b=(i>>index%BYTESIZE)%2;
    *value=(BYTE)b;
#else
    comedi_dio_read(it, Comedi_subdev_i, index, &b);
    *byte = (BYTE) b;
#endif
}
void dio_write(const BYTE * value,const int n,const int bit)
{	//write bit to n output
	BYTE q;
    q = value[n / BYTESIZE];
	q |= bit << n % BYTESIZE;
#ifndef USE_COMEDI
	outb(q,Base+Wr_offs+n/BYTESIZE);
#else
	comedi_dio_write(it, Comedi_subdev_q, n, bit);
#endif
}
void dio_bitfield(const unsigned char *mask, unsigned char *bits)
{	//simultaneusly write output bits defined my mask and read all inputs
    int i;//, j, n, in;

#ifndef USE_COMEDI
	for(i=0;i<Dq;i++)
    outb(bits[i] & mask[i], Base+Wr_offs+i);
	for(i=0;i<Di;i++)
	bits[i]=inb(Base+Rd_offs+i);
#else
    unsigned int w, b;
    w = (unsigned int) (*mask);
	b = (unsigned int) (*bits);
	comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);
#endif
}
