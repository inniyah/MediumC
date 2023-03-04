/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   error.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

#define ERROR   0
#define WARNING 1

/* local functions */
void domsg(int typ, char *err);

int errcnt=0;                    /* error counter */
int warncnt=0;                   /* warning counter */

/**********************************************************
 * print error message
 * error should kill the offending line
 ***********************************************************/
void error(char *err)
{
    FILE *tempfile;

    tempfile=output;
    output=stdout;
    domsg(ERROR,err);            /* output to screen */
    output=tempfile;
    domsg(ERROR,err);            /* output to file */
    if(MAXERR<errcnt++) {
        comment();
        outstr("To many errors, aborting..");
        nl();
        fcloseall();
        exit(4);
    }
}


/**********************************************************
 * print warning message
 ***********************************************************/
void warning(char *warn)
{
    FILE *tempfile;

    tempfile=output;
    output=stdout;
    domsg(WARNING,warn);         /* output to screen */
    output=tempfile;
    domsg(WARNING,warn);         /* output to file */
    warncnt++;                   /* count warnings */
}


/**********************************************************
 * print fatal error message and exit to dos
 * reserved for compiler internal errors
 ***********************************************************/
void fatal(char *err, int code)
{
    fprintf(stderr, "Fatal error: %s\naborting..", err);
    fcloseall();                 /* close files */
    exit(code);                  /* exit to OS */
}


/***********************************************************
 * output error/warning message
 ***********************************************************/
void domsg(int typ, char *err)
{
    int k=0;

    comment();
    outstr(line);
    nl();
    comment();
    while(k<lptr) {              /* point to offending place */
        if(line[k]==TAB)
            tab();
        else
            outbyte(' ');
        k++;
    }
    outbyte('^');
    nl();
    comment();
    outstr(typ==ERROR ? "Error:  ":"Warning: ");
    outstr(err);
    nl();
}


/**********************************************************
 *	report errors
 * VERIFY
 **********************************************************/
void errorsummary(void)
{
    entry *ptr;
    int i;

    if(ncmp) error("missing closing bracket");
    nl();
    comment();
    outdec(errcnt);
    if(errcnt) errfile=TRUE;
    outstr(" error(s) in compilation");
    nl();
    comment();
    ot("literal pool:");
    outdec(litptr);
    nl();
    comment();
    ot("global pool:");
    i=1;
    ptr=glbtab->start;
    while(ptr) {                 /* mozna inaczej!! */
        i++;
        ptr=ptr->next;
    }
    outdec(i);
    nl();
    comment();
    ot("Macro pool:");

    /*
    k=macstart;
    i=0;
    while(k<macptr) if(*k++==0) i++;
    */

    outdec(i/2);
    nl();
    fputs(errcnt ? "Error(s)\n" : "No errors\n",stderr);
}
