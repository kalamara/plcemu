# Makefile for PLCEMU
# (c) 2014

AM_CFLAGS=-Wall


bin_PROGRAMS=plcemu 

plcemu_SOURCES= plcemu.c plcemu.h plclib.c plclib.h parser.c parser.h hardware.h ui.h project.h project.c
#NCURSES GUI
if UI
plcemu_SOURCES+= ui-nc.c greek.c greek.h args.c args.h wselect.c wedit.h
else
plcemu_SOURCES+= ui-std.c
endif
#hardware modules supported
if SIMULATION
plcemu_SOURCES+=hardware-sim.c
else 
if USPACE
plcemu_SOURCES+=hardware-uspace.c
else
plcemu_SOURCES+=hardware-comedi.c
endif
endif

#testhw_SOURCES=testhw.c hardware.h plcemu.h
#testil_SOURCES=
#teststack_SOURCES=

ui-nc.o: ui-nc.c ui.h
hardware-comedi.o: hardware-comedi.c hardware.h
hardware-uspace.o: hardware-uspace.c hardware.h
hardware-sim.o: hardware-sim.c hardware.h

nodes:; mknod plcpipe p; mknod plcresponse p; 



