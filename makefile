# MediumC. TurboC Makefile

objfile = scrap\data.obj scrap\error.obj scrap\expr.obj\
	  scrap\function.obj scrap\gen.obj scrap\io.obj\
	  scrap\lex.obj scrap\main.obj scrap\preproc.obj\
	  scrap\primary.obj scrap\stmt.obj scrap\sym.obj\
	  scrap\glbsym.obj scrap\locsym.obj scrap\while.obj\
	  scrap\vm.obj scrap\code.obj scrap\listmng.obj

#cc compiler:
cc     = c:\tc\tcc                      #edit for compiler
cflags = -N -y -c -O -Z -Ic:\tc\include -mm -oscrap\$&

#linker
lnkp    = c:\tasm\tlink                 #edit for linker
tlib    = c:\tc\lib                     #edit for libraries
lflags  = /m /c /d                      #linker options
mapfile = scrap\mcc                     #map file
init    = $(tlib)\c0m                   #startup
libs    = $(tlib)\cm $(tlib)\emu        #libraries

#link compiler to mcc.exe:
mcc.exe: $(objfile) 
	$(lnkp) $(lflags) $(init) @linkfile,mcc,$(mapfile),$(libs)

#all objects:
scrap\data.obj: data.c defs.h data.h externs.h
	$(cc) $(cflags) data.c

scrap\error.obj: error.c defs.h data.h externs.h
	$(cc) $(cflags) error.c

scrap\expr.obj: expr.c defs.h data.h externs.h
	$(cc) $(cflags) expr.c

scrap\function.obj: function.c defs.h data.h externs.h
	$(cc) $(cflags) function.c

scrap\gen.obj: gen.c defs.h data.h externs.h
	$(cc) $(cflags) gen.c

scrap\io.obj: io.c defs.h data.h externs.h
	$(cc) $(cflags) io.c

scrap\lex.obj: lex.c defs.h data.h externs.h
	$(cc) $(cflags) lex.c

scrap\main.obj: main.c defs.h data.h externs.h
	$(cc) $(cflags) main.c

scrap\preproc.obj: preproc.c defs.h data.h externs.h
	$(cc) $(cflags) preproc.c

scrap\primary.obj: primary.c defs.h data.h externs.h
	$(cc) $(cflags) primary.c

scrap\stmt.obj: stmt.c defs.h data.h externs.h
	$(cc) $(cflags) stmt.c

scrap\sym.obj: sym.c defs.h data.h externs.h
	$(cc) $(cflags) sym.c

scrap\glbsym.obj: glbsym.c defs.h data.h externs.h
	$(cc) $(cflags) glbsym.c

scrap\locsym.obj: locsym.c defs.h data.h externs.h
	$(cc) $(cflags) locsym.c

scrap\while.obj: while.c defs.h data.h externs.h
	$(cc) $(cflags) while.c 

scrap\vm.obj: vm.c defs.h data.h externs.h
	$(cc) $(cflags) vm.c

scrap\code.obj: code.c defs.h data.h externs.h
	$(cc) $(cflags) code.c

scrap\listmng.obj: listmng.c defs.h data.h externs.h
	$(cc) $(cflags) listmng.c
