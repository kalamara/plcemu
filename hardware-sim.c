#include "plcemu.h"
#include <poll.h>
#include <fcntl.h>
#include "hardware.h"


FILE * Ifd = NULL;
FILE * Qfd = NULL;
BYTE * BufIn = NULL;
BYTE * BufOut = NULL;

int enable_bus() /* Enable bus communication */
{
    int r = 1;
    /*open input and output streams*/
    if(!(Ifd=fopen(SimInFile, "r+")))
        r = -1;
    else
        plclog("Opened simulation input");

    if(!(Qfd=fopen(SimOutFile, "w+")))
        r = -1;
    else
        plclog("Opened simulation output");

    if(!(BufIn = (BYTE * )malloc(sizeof(BYTE)*Di)))
        r = -1;
    else
        memset(BufIn, 0, sizeof(BYTE)*Di);

    if(!(BufOut = (BYTE * )malloc(sizeof(BYTE)*Dq)))
        r = -1;
    else
        memset(BufOut, 0, sizeof(BYTE)*Dq);
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
    {
        free(BufIn);
        BufIn = NULL;
    }
    if(BufOut)
    {
        free(BufOut);
        BufOut = NULL;
    }
    return r;
}

int dio_fetch(long timeout)
{
    int bytesRead = 0;
    bytesRead = fread(BufIn, sizeof(BYTE), Di, Ifd?Ifd:stdin);
    int i = 0;
    for(i = 0; i < bytesRead; i++)
        if(BufIn[i] >= ASCIISTART)
            BufIn[i] -= ASCIISTART;
    //plclog("read %d bytes after %ld ms from %s", bytesRead, timeout, SimInFile);
    if(bytesRead < Di)
    {
        disable_bus();
        enable_bus();
    }
    return bytesRead;
}

int dio_flush()
{
    int bytesWrote = 0;
    bytesWrote = fwrite(BufOut, sizeof(BYTE), Dq, Qfd?Qfd:stdout);
    fputc('\n',Qfd?Qfd:stdout);
    fflush(Qfd);
    //plclog("wrote %d bytes ms to %s:%s", bytesWrote, SimOutFile, BufOut);
    return bytesWrote;
}

void dio_read(int n, BYTE* bit)
{	//write input n to bit
    unsigned int b, position;
    position = n / BYTESIZE;
    BYTE i = 0;
    if(strlen(BufIn) > position)
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
    q |= bit << n % BYTESIZE;
    /*write a byte to output stream*/
    q+=ASCIISTART; //ASCII
   // plclog("Send %d to byte %d", q, position);
     if(strlen(BufOut) >= position)
         BufOut[position] = q;
}

void dio_bitfield(const unsigned char* mask, unsigned char *bits)
{	//simultaneusly write output bits defined by mask and read all inputs
    /* FIXME
    int i=0;
    unsigned int w = (unsigned int) (*mask);
    unsigned int b = (unsigned int) (*bits);
    comedi_dio_bitfield(it, Comedi_subdev_i, w, &b);*/
}
