# Makefile for the Arc/Info Vector Coverage (AVC) reader 
#
# $Id: GNUmakefile,v 1.8 2006/06/27 18:38:43 dmorissette Exp $
#
# $Log: GNUmakefile,v $
# Revision 1.8  2006/06/27 18:38:43  dmorissette
# Cleaned up E00 reading (bug 1497, patch from James F.)
#
# Revision 1.7  2001/11/25 21:15:22  daniel
# Added hack (AVC_MAP_TYPE40_TO_DOUBLE) to map type 40 fields bigger than 8
# digits to double precision as we generate E00 output (bug599)
#
# Revision 1.6  2000/09/26 20:48:40  daniel
# Added optional -DAVC_IGNORE_RMDIR_ERROR
#
# Revision 1.5  2000/05/29 17:12:56  daniel
# Added avc_mbyte.*
#
# Revision 1.4  1999/12/24 07:18:34  daniel
# Added PC Arc/Info coverages support
#
# Revision 1.3  1999/05/11 02:06:54  daniel
# Added coverage write support (alpha version)
#
# Revision 1.2  1999/02/25 05:19:06  daniel
# Added cpl_dir.c
#
# Revision 1.1  1999/01/29 16:29:55  daniel
# Initial revision
#
#

#
#  Default compiler is gcc, 
#
CC      =       gcc

#
# Link with DBMALLOC when running in a test environment
#
#LIB_DBMALLOC= /home/daniel/proj/dbmalloc-14L2/libdbmalloc.a

#
#  Byte ordering ... default is Intel ordering (CPL_LSB),
#  Select CPL_MSB for SUNs or other platforms with MSB first ordering.
#
# BYTE_ORDER_FL = -DCPL_MSB
BYTE_ORDER_FL = -DCPL_LSB

#
# In some cases, AVCE00DeleteCoverage() fails because the coverage directory
# could be locked by another application on the same system or somewhere on
# the network.  
# Uncomment the following line to define AVC_IGNORE_RMDIR_ERROR at compile 
# time if you want this error to be ignored.
#
# OPTFLAGS := $(OPTFLAGS) -DAVC_IGNORE_RMDIR_ERROR

#
# Due to the odd way that fields of type 40 are handled in E00, you will
# start to lose some digits of precision with fields bigger than 8 digits when 
# exporting to E00.  Define AVC_MAP_TYPE40_TO_DOUBLE to ask the AVC lib to
# automatically remap fields of type 40 bigger than 8 digits to double 
# precision floats while writing E00.   Double precision floats can carry up 
# to 18 digits of precision.
#
OPTFLAGS := $(OPTFLAGS) -DAVC_MAP_TYPE40_TO_DOUBLE


#
#  
#
CFLAGS= $(BYTE_ORDER_FL) $(OPTFLAGS) -g -Wall -DDEBUG
LFLAGS= 
AR= ar

LIB_OBJS= avc_e00read.o avc_e00write.o avc_rawbin.o avc_bin.o avc_binwr.o \
	  avc_e00gen.o avc_e00parse.o avc_misc.o \
	  cpl_error.o cpl_conv.o cpl_vsisimple.o cpl_string.o cpl_dir.o \
	  avc_mbyte.o dbfopen.o

LIB=      avc.a

default: $(LIB) avcimport avcexport avcdelete avctest ex_avcwrite

$(LIB_OBJS):	avc.h avc_mbyte.h cpl_error.h cpl_port.h cpl_conv.h \
		cpl_vsi.h cpl_string.h

avcimport: avcimport.o $(LIB) avc.h
	$(CC) $(LFLAGS) -o avcimport avcimport.o $(LIB)

avcexport: avcexport.o $(LIB) avc.h
	$(CC) $(LFLAGS) -o avcexport avcexport.o $(LIB)

avcdelete: avcdelete.o $(LIB) avc.h
	$(CC) $(LFLAGS) -o avcdelete avcdelete.o $(LIB)

avctest: avctest.o $(LIB) avc.h
	$(CC) $(LFLAGS) -o avctest avctest.o $(LIB)

ex_avcwrite: ex_avcwrite.o $(LIB) avc.h
	$(CC) $(LFLAGS) -o ex_avcwrite ex_avcwrite.o $(LIB)  $(LIB_DBMALLOC)

testmulti: testmulti.o $(LIB) avc.h
	$(CC) $(LFLAGS) -o testmulti testmulti.o $(LIB) $(LIB_DBMALLOC)

%.o:	%.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o

%.o:	%.cpp
	$(CC) -c $(CFLAGS) $*.cpp -o $*.o

$(LIB): $(LIB_OBJS)
	rm -f $(LIB)
	$(AR) rc $(LIB) $(LIB_OBJS)

clean:
	rm -f $(LIB) $(LIB_OBJS)
	rm -f avctest.o avctest avcexport.o avcexport avcimport.o avcimport \
	ex_avcwrite ex_avcwrite.o avcdelete.o avcdelete 

