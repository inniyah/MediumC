#!/usr/bin/make -f

PROGRAM= mcc

ARCH_NAME := $(shell '$(TRGT)gcc' -dumpmachine)

PACKAGES= 

PKGCONFIG= pkg-config

ifndef PACKAGES
PKG_CONFIG_CFLAGS=
PKG_CONFIG_LDFLAGS=
PKG_CONFIG_LIBS=
else
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PACKAGES)`
PKG_CONFIG_LDFLAGS=`pkg-config --libs-only-L $(PACKAGES)`
PKG_CONFIG_LIBS=`pkg-config --libs-only-l $(PACKAGES)`
endif

CFLAGS= \
	-Wall \
	-fwrapv \
	-fstack-protector-strong \
	-Wall \
	-Wformat \
	-Werror=format-security \
	-Wdate-time \
	-D_FORTIFY_SOURCE=2 \
	-fPIC

LDFLAGS= \
	-Wl,-O1 \
	-Wl,-Bsymbolic-functions \
	-Wl,-z,relro \
	-Wl,--as-needed \
	-Wl,--no-undefined \
	-Wl,--no-allow-shlib-undefined \
	-Wl,-Bsymbolic-functions \
	-Wl,--dynamic-list-cpp-new \
	-Wl,--dynamic-list-cpp-typeinfo

CSTD=-std=gnu17

CPPSTD=-std=gnu++17

OPTS= -O2 -g

DEFS= \
	-DNDEBUG \
	-D_GNU_SOURCE \
	-D_LARGEFILE64_SOURCE \
	-D_FILE_OFFSET_BITS=64

INCS= 

LIBS= 

OBJS= \
	data.o \
	error.o \
	expr.o \
	function.o \
	gen.o \
	io.o \
	lex.o \
	main.o \
	preproc.o \
	primary.o \
 	stmt.o \
	sym.o \
	glbsym.o \
	locsym.o \
	while.o \
	vm.o \
	code.o \
	listmng.o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CPPSTD) $(CSTD) $(LDFLAGS) $(PKG_CONFIG_LDFLAGS) -o $@ $+ $(LIBS) $(PKG_CONFIG_LIBS)

lib%.so:
	$(LD) -shared $(CPPSTD) $(CSTD) $(LDFLAGS) $(PKG_CONFIG_LDFLAGS) -o $@ $^ $(LIBS) $(PKG_CONFIG_LIBS)

%.a:
	$(AR) $@ $^

%.o: %.cpp
	$(CXX) $(CPPSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	$(CC) $(CSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

clean:
	rm -fv $(OBJS) $(PROGRAM)
