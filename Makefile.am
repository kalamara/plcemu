# Makefile for PLCEMU
# (c) 2018

AM_CFLAGS=-g -O0 -Wall -I./src -I./src/vm -I./src/ui -I./src/hw

bin_PROGRAMS=plcemu 

lib_LTLIBRARIES=libyacf.la
libyacf_la_SOURCES= src/config.c src/config.h \
                    src/util.c src/util.h 

plcemu_SOURCES= src/plcemu.c src/plcemu.h \
                src/vm/plclib.c src/vm/plclib.h \
                src/vm/parser-il.c src/vm/parser-il.h \
                src/vm/parser-ld.c src/vm/parser-ld.h \
                src/vm/parser-tree.c src/vm/parser-tree.h \
                src/vm/codegen.c src/vm/codegen.h \
                src/vm/data.c src/vm/data.h \
                src/vm/instruction.c src/vm/instruction.h \
                src/vm/rung.c src/vm/rung.h \
                src/hw/hardware.h \
                src/ui/ui.h \
                src/project.h src/project.c
plcemu_LDFLAGS=-lyacf

#ZMQ server for GUI
if UI
plcemu_SOURCES+= src/ui/ui-zmq.c
bin_PROGRAMS+=cli
cli_SOURCES=src/ui/cli-zmq.c
cli_LDFLAGS=-lyacf
else
plcemu_SOURCES+= src/ui/ui-std.c
endif
#hardware modules supported
if SIMULATION
plcemu_SOURCES+=src/hw/hardware-sim.c
else 
if USPACE
plcemu_SOURCES+=src/hw/hardware-uspace.c
else
plcemu_SOURCES+=src/hw/hardware-comedi.c
endif
endif

nodes:; mknod plcpipe p; mknod plcresponse p; 



