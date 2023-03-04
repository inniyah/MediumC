/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   data.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>

#include "defs.h"

/* symbol storage */
list *glbtab;                    /* start of global symbol table */
list *loctab;                    /* start of local symbol table */
macentry *macstart;              /* start of macro definition table */

wsentry *startws;                /* start of do/for/while/switch table */
wsentry *endws;                  /* end of do/for/while/switch table */
wsentry *wsptr;                  /* ptr to do/for/while/switch table */

/* better use pointers, not arrays!.. */
int swstcase[SWSTSZ];
int swstlab[SWSTSZ];
int swstp;

char    litq[LITABSZ];
int litptr;

char line[LINESIZE];             /* input data buffer */
int lptr;                        /* pointer to data in input buffer */

/* miscellaneous storage */
int litlab;
int stkp;                        /* stack pointer */
int ncmp;
int fexitlab;                    /* storage for function exit label */

/* code generation switches */
int ctext;                       /* append source to output */
int cmode;                       /* signal #asm mode */
int aflag;                       /* indicate #of function args */

/* files used */
FILE *input;                     /* source file */
FILE *input2;                    /* include file */
FILE *output;                    /* assembler file */

/* use linked list instead!.. */
FILE    *inclstk[INCLSIZ];       /* for multiple #include directives */
int inclsp;                      /* ptr to a particular file */

int errfile;                     /* write the code!.. */

int TRACE;                       /* only for debuging */
