/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   io.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/**********************************************************
 * kill a line
 **********************************************************/
void kill(void)
{
    lptr=0;
    line[lptr]=NULL;
}


/**********************************************************
 * fills an input buffer with chars from input file
 * if needed append line to output file
 **********************************************************/
void finline(void)
{
    int k;
    FILE    *unit;

    while(1) {
        if(feof(input)) return;
        if((unit=input2)==NULL) unit=input;
        kill();
        while((k=fgetc(unit))!=EOF) {
            if((k==EOL) || (lptr>=LINEMAX)) break;
            line[lptr++]=k;
        }
        line[lptr]=0;
        if(k<=0)
        if(input2!=NULL) {
            input2=inclstk[--inclsp];
            fclose(unit);
        }
        if(lptr) {
            if((ctext==TRUE) && (cmode==TRUE)) {
                comment();
                outstr(line);
                nl();
            }
            lptr=0;
            return;
        }
    }
}


/*********************************************************
 * preprocess input, return char from buffer and advance
 * line pointer
 **********************************************************/
char inchar(void)
{
    while(ch()==0) {
        if(feof(input)) return(0);
        preprocess();
    }
    return(gch());
}


/*********************************************************
 * return char from file or NULL if EOF
 **********************************************************/
char inbyte(void)
{
    if(ch()==0) finline();
    if(feof(input)) return(0);
    return(gch());
}


/**********************************************************
 * return current char from line or NULL
 * advance line ptr
 **********************************************************/
char gch(void)
{
    if(ch()==0) return(0);
    else
        return(line[lptr++]&0x7f);
}


/**********************************************************
 * return next char from line or NULL
 * do not advance line ptr
 **********************************************************/
char nch(void)
{
    if(ch()==0) return(0);
    else
        return(line[lptr+1]&0x7f);
}


/**********************************************************
 * return char from line
 * do not advance line ptr
 **********************************************************/
char ch(void)
{
    return(line[lptr]&0x7f);
}
