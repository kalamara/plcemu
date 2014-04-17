# Makefile for PLC_EMU
# (c) 2007 Polaris Engineering S. A.

CFLAGS += -O -g


all:		plcemu
		
project:	project.o plcemu
			
nocomedi:	plcemu.o hardware-nocomedi.o plclib.o parser.o  project.o wselect.o greek.o args.o
		$(CC) $(CFLAGS) -o plcemu plcemu.o hardware-nocomedi.o parser.o plclib.o project.o wselect.o greek.o args.o  -lncurses -lm

ultralite:	plcemu.o hardware.o plclib.o project.o parser.o wselect.o greek.o args.o
		$(CC) $(CFLAGS) -DULTRALITE=1 -o plcemu plcemu.o parser.o hardware.o plclib.o project.o wselect.o greek.o args.o -lm
testhw:		testhw.o
		$(CC) $(CFLAGS) -o testhw testhw.o -lcomedi -lm
testhw.o:	testhw.c hardware.h
		$(CC) $(CFLAGS) -c  testhw.c
plcemu:		plcemu.o hardware.o plclib.o project.o parser.o wselect.o greek.o args.o
		$(CC) $(CFLAGS) -o plcemu plcemu.o hardware.o plclib.o project.o wselect.o parser.o greek.o args.o  -lcomedi -lncurses -lm
plcemu.o:	plcemu.c plcemu.h 
		$(CC) $(CFLAGS) -c  plcemu.c
		
hardware.o:	hardware.c hardware.h
                $(CC) $(CFLAGS) -DUSE_COMEDI=0 -c hardware.c
		
hardware-nocomedi.o: hardware-nocomedi.c hardware.h
		$(CC) $(CFLAGS) -c hardware.c

plclib.o:	plclib.c plclib.h
		$(CC) $(CFLAGS) -c  plclib.c 
project.o:	project.c project.h 
		$(CC) $(CFLAGS) -c  project.c
args.o:		args.c args.h
		$(CC) $(CFLAGS) -c args.c
config.o:	config.c args.h
		$(CC) $(CFLAGS) -c config.c
greek.o:	greek.c greek.h
		$(CC) $(CFLAGS) -c greek.c
wselect.o:	wselect.c  wedit.h
		$(CC) $(CFLAGS) -c wselect.c
parser.o:	parser.c parser.h
		$(CC) $(CFLAGS) -c parser.c
teststack:	teststack.o
		$(CC) $(CFLAGS) -o teststack teststack.o parser.o plclib.o
teststack.o:	teststack.c
		$(CC) $(CFLAGS) -c  teststack.c

testil:		testil.o
		$(CC) $(CFLAGS) -o testil testil.o parser.o plclib.o
testil.o:	testil.c
		$(CC) $(CFLAGS) -c  testil.c

clean:
		rm -f *.o plcemu testhw
install:
		chmod +s plcemu; killall plcemu; cp plcemu /usr/bin; mknod plcpipe p; mknod plcresponse p;
