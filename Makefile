# Makefile for KOS -- the derivative of KOS (Jim Plank's OS)
# http://www.cs.utk.edu/~plank
#
# Michael A. Puening modified by Kim Buckner Spring 97
# further modified for kim's lab4
#
# modified for cs560 James Spring 00 
#
# modified for UCSB, Spring 2002
#
# modified for UCSB further, Fall 2007 and 2008
# 
# modified for UCSB further, Winter 2014


CC = gcc -m32
OS=linux
KHOME = /cs/faculty/rich/cs170
CFLAGS = -g -I$(KHOME)/include -I.

LIB = $(KHOME)/lib/libsim.a $(KHOME)/lib/libkt.a $(KHOME)/lib/libfdr.a $(KHOME)/lib/libkos.a
#LIB = -L$(KOSDIR)/lib -lsim -lkt$(LIBCVERSION) -lfdr -lnsl
MAIN = $(KHOME)/lib/main_lab2.o $(KHOME)/lib/rb.o

EXECUTABLES = kos

all: $(EXECUTABLES)

INCL = scheduler.h console_buf.h syscall.h memory.h kos.h
CFILES = scheduler.c console_buf.c syscall.c memory.c kos.c exception.c

USER    = exception.o kos.o scheduler.o console_buf.o syscall.o memory.o

kos: $(USER) $(MAIN) $(LIB) $(INCL) 
	$(CC) $(CFLAGS) -g -o kos $(MAIN) $(USER) $(LIB)

.c.o: $(INCL) $(CFILES)
	$(CC) -c $(CFLAGS) $*.c

clean:
	/bin/rm -f *.o core $(EXECUTABLES) 

