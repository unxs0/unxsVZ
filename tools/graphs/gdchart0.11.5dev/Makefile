CC=gcc
# gcc 2.7.1 or better is required
# CFLAGS=
# CFLAGS=-g -ansi -pedantic

# ----- build path -----
GDC_INCL=./
GDC_LD=./
GDC_LIB=libgdc.a

# ----- install locations -----
PREFIX_INC = /usr/include
PREFIX_LIB = /usr/lib

# INCLUDEDIRS=-I. -I/usr/include/freetype2 -I/usr/include/X11 -I/usr/X11R6/include/X11 -I/usr/local/include 

# ----- lib gd -----
# GDChart requires the gd library - www.boutell.com/gd/
# gd 2.0.28 or better is required (GIF support has returned to libgd)
# if it's not installed in a standard location edit these lines for your installation
GD_INCL=/usr/include/
GD_LD=/usr/lib/
GD_LIB=libgd.so
# a static libgd is also available
# GD_LIB=libgd.a

# ----- lib png -----
# libgd requires libpng
# if it's not installed in a standard location edit these lines for your installation
# PNG_INCL = ../libpng-1.0.8
# PNG_LD   = ../libpng-1.0.8

# ----- lib z -----
# libgd requires zlib
# if it's not installed in a standard location edit these lines for your installation
# ZLIB_INCL = ../zlib-1.1.3
# ZLIB_LD   = ../zlib-1.1.3

# ----- lib jpeg -----
# libgd optionally uses libjpeg to produce JPEG images
# JPEG_INCL = ../libjpeg
# JPEG_LD   = ../libjpeg
JPEG_DEF  = -DHAVE_JPEG
JPEG_LK   = -ljpeg

# libgd optionally uses libfreetype to use TTFs
# FT_LD  = /usr/local/lib
FT_DEF    = -DHAVE_LIBFREETYPE
FT_LK     = -lfreetype

DEFS = $(FT_DEF) $(JPEG_DEF)
LIBS = $(FT_LK) $(JPEG_LK)

LIB_PATHS   = -L$(GD_LD) -L$(GDC_LD)
# if not installed in standard paths (/lib, /usr/lib), or LD_LIBRARY_PATH
# LIB_PATHS   = -L$(GD_LD) -L$(PNG_LD) -L$(ZLIB_LD) -L$(JPEG_LD)

# NOTE:
# libpng, libz, etc. are usually linked in as dynamic libs
# either use a link line similar to one of these, or set environment LD_LIBRARY_PATH
# these should likely be absolute paths, as resolved at runtime
# LIB_PATHS = $(LIB_PATHS) -R$(PNG_LD) -R$(ZLIB_LD)
# LIB_PATHS = $(LIB_PATHS) -Xlinker -rpath -Xlinker $(PNG_LD) -Xlinker -rpath -Xlinker $(ZLIB_LD)


all: libgdc.a

# --- compile the lib ---
gdc.h: $(GD_INCL)gd.h $(GD_INCL)gdfonts.h $(GD_INCL)gdfontt.h $(GD_INCL)gdfontmb.h $(GD_INCL)gdfontg.h $(GD_INCL)gdfontl.h $(GDC_INCL)array_alloc.h

price_conv.o: price_conv.c
	$(CC) $(CFLAGS) -c price_conv.c

gdc.o: gdc.c $(GDC_INCL)gdc.h
	$(CC) $(CFLAGS) -I$(GD_INCL) -I$(GDC_INCL) $(DEFS) -c gdc.c

gdc_pie.o: $(GDC_INCL)gdc.h $(GDC_INCL)gdcpie.h gdc_pie.c
	$(CC) $(CFLAGS) -I$(GD_INCL) -I$(GDC_INCL) $(DEFS) -c gdc_pie.c

gdchart.o: $(GDC_INCL)gdc.h $(GDC_INCL)gdchart.h gdchart.c
	$(CC) $(CFLAGS) -I$(GD_INCL) -I$(GDC_INCL) $(DEFS) -c gdchart.c

array_alloc.o: array_alloc.c array_alloc.h
	$(CC) $(CFLAGS) -c array_alloc.c

libgdc.a: price_conv.o gdc.o gdc_pie.o gdchart.o array_alloc.o
	ar cr libgdc.a gdc.o gdchart.o gdc_pie.o price_conv.o array_alloc.o
	ranlib libgdc.a

# --- supporting libraries ---
# should be compile & installed separately
# $(GD_LD)/$(GD_LIB):
# 	cd $(GD_LD) ; make -f Makefile $(GD_LIB)

# ----- install -----
install: gdc.h gdchart.h gdcpie.h libgdc.a
	cp gdc.h  gdchart.h  gdcpie.h $(PREFIX_INC)/
	cp libgdc.a $(PREFIX_LIB)/

# --- clean ---
clean:
	rm -f *.o *.a

