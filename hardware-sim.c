#include "plcemu.h"
#include "hardware.h"

FILE * Ifd = NULL;
FILE * Qfd = NULL;
BYTE * BufIn;
BYTE * BufOut;

int enable_bus() /* Enable bus communication */
{
    int r = 1;
    /*open input and output streams*/
    if(!(Ifd=fopen(SimInFile, "r+")))
        r = -1;
    if(!(Qfd=fopen(SimOutFile, "w+")))
        r = -1;
    if(!(BufIn = (BYTE * )malloc(sizeof(BYTE)*Di)))
        r = -1;
    if(!(BufOut = (BYTE * )malloc(sizeof(BYTE)*Dq)))
        r = -1;
    return r;
}

int disable_bus() /* Disable bus communication */
{
    int r = 1;
    /*close streams*/
    if(!fclose(Ifd))
        r = -1;
    if(!fclose(Qfd))
        r = -1;
    if(BufIn)
        free(BufIn);
    if(BufOut)
        free(BufOut);

    return r;
}

int dio_fetch()
{
    int bytesRead = 0;

    if(BufIn)
        bytesRead = fread(BufIn, sizeof(BYTE), Di/BYTESIZE, Ifd?Ifd:stdin);

    return bytesRead;
}

int dio_flush()
{
    int bytesWrote = 0;

    if(BufOut)
        bytesWrote = fwrite(BufOut, sizeof(BYTE), Dq/BYTESIZE, Qfd?Qfd:stdout);

    return bytesWrote;
}

void dio_read(int n, BYTE* bit)
{	//write input n to bit
    unsigned int b, position;
    position = n / BYTESIZE;
    BYTE i = 0;
    if(BufIn && strlen(BufIn) > position)
    /*read a byte from input stream*/
        i = BufIn[position];
	b = (i >> n % BYTESIZE) % 2;
	*bit = (BYTE) b;
}

void dio_write(const unsigned char *buf, const int n, const int bit)

{	//write bit to n output
	BYTE q;
    unsigned int position = n / BYTESIZE;
    q = buf[position];
    //q |= bit << n % BYTESIZE;
    /*write a byte to output stream*/
     if(BufOut && strlen(BufOut) > position)
         BufOut[position] = buf[position]; //should we just mask the bit instead of copying the whole byte?
}

void dio_bitfield(const unsigned char* mask, unsigned char *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
    /* FIXME
    int i=0;
    unsigned int w = (unsigned int) (*mask);
    unsigned int b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}
