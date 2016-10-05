# Makefile for the Compressed E00 Read/Write library 
#
# $Id: Makefile,v 1.2 1999/02/25 18:57:09 daniel Exp $
#
# $Log: Makefile,v $
# Revision 1.2  1999/02/25 18:57:09  daniel
# Now use CPL for Error handling, Memory allocation, and File access
#
# Revision 1.1  1999/01/08 17:41:17  daniel
# Initial revision
#
#


CC= gcc
CFLAGS= -Wall
LFLAGS= 
AR= ar

LIB_OBJS= e00read.o e00write.o cpl_error.o cpl_conv.o cpl_vsisimple.o
LIB=      e00compr.a

default: all

all: e00conv

$(LIB_OBJS): e00compr.h

e00conv: e00conv.o $(LIB)
	$(CC) $(LFLAGS) -o e00conv e00conv.o $(LIB)

ex_read: ex_read.o $(LIB)
	$(CC) $(LFLAGS) -o ex_read ex_read.o $(LIB)

ex_readcb: ex_readcb.o $(LIB)
	$(CC) $(LFLAGS) -o ex_readcb ex_readcb.o $(LIB)

ex_write: ex_write.o $(LIB)
	$(CC) $(LFLAGS) -o ex_write ex_write.o $(LIB)

ex_writecb: ex_writecb.o $(LIB)
	$(CC) $(LFLAGS) -o ex_writecb ex_writecb.o $(LIB)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(LIB): $(LIB_OBJS)
	rm -f $(LIB)
	$(AR) rc $(LIB) $(LIB_OBJS)

clean:	clean-examples
	rm -f e00conv e00conv.o $(LIB) $(LIB_OBJS)

clean-examples:
	rm -f ex_read ex_read.o ex_readcb ex_readcb.o
	rm -f ex_write ex_write.o ex_writecb ex_writecb.o
