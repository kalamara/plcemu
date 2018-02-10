# Makefile for PLCEMU
# (c) 2018

AM_CFLAGS=-g -O0 -Wall -I./src -I./src/vm -I./src/ui -I./src/hw

bin_PROGRAMS=plcemu 


plcemu_SOURCES= src/plcemu.c src/plcemu.h \
                src/app.c src/app.h \
                src/vm/plclib.c src/vm/plclib.h \
                src/vm/parser-il.c src/vm/parser-il.h \
                src/vm/parser-ld.c src/vm/parser-ld.h \
                src/vm/parser-tree.c src/vm/parser-tree.h \
                src/vm/codegen.c src/vm/codegen.h \
                src/vm/data.c src/vm/data.h \
                src/vm/instruction.c src/vm/instruction.h \
                src/vm/rung.c src/vm/rung.h \
                src/hw/hardware.h src/hw/hardware.c \
                src/hw/hardware-sim.c \
                src/hw/hardware-uspace.c \
                src/hw/hardware-comedi.c \
                src/config.c src/config.h \
                src/util.c src/util.h \
                src/ui/ui.h src/ui/cli.c\
                src/project.h src/project.c

#ZMQ server for GUI
if UI
plcemu_SOURCES+= src/ui/ui-zmq.c
bin_PROGRAMS+=cli
cli_SOURCES=src/ui/cli-zmq.c \
            src/ui/cli.c \
            src/ui/ui-zmq.c \
            src/config.c src/config.h \
            src/util.c src/util.h 
else
plcemu_SOURCES+= src/ui/ui-std.c
endif

#hardware modules supported

if SIMULATION
AM_CFLAGS+=-DSIM
endif

if COMEDI
AM_CFLAGS+=-DCOMEDI
endif

nodes:; mknod plcpipe p; mknod plcresponse p; 



