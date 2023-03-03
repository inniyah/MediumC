/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   data.h
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
/* storage words */

extern list *glbtab;		/* start of global symbol table */
extern list *loctab;		/* start of local symbol table */

extern macentry *macstart;	/* start of macro definition table */

extern wsentry *startws;/* start of do/for/while/switch table */
extern wsentry *endws;	/* end of do/for/while/switch table */
extern wsentry *wsptr;	/* ptr to do/for/while/switch table */

extern int swstcase[];
extern int swstlab[];
extern int swstp;
extern char litq[];
extern int litptr;

extern char line[LINESIZE];	/* input file buffer */
extern int lptr;	/* pointer into line[] */


/* miscellaneous storage */

extern int litlab;
extern int stkp;				/* stack pointer */
extern int ncmp;
extern int fexitlab;		/* temp storage for function exit label */

/* code generation switches */
extern int ctext;				/* append source to output */
extern int cmode;				/* signal #asm mode */
extern int aflag;				/* indicate #of function args */

/* files used */
extern FILE *input;
extern FILE *input2;
extern FILE *output;
extern FILE *inclstk[];	/* for #include stack */
extern int inclsp;

extern int errfile;			/* uzupelnic */

extern int TRACE;				/* only for compiler debugging */
