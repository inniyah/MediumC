/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   while.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/********************************************************
change to a linked list!!
**********************************************************/
void addwhile(wsentry ws)
{
    if(wsptr>=endws) {
        error("too many active whiles");
        return;
    }
    wsptr->wssym=ws.wssym;
    wsptr->wssp=ws.wssp;
    wsptr->wstyp=ws.wstyp;
    wsptr->wstest=ws.wstest;
    wsptr->wsdef=ws.wsdef;
    wsptr->wstab=ws.wstab;
    wsptr->wsexit=ws.wsexit;
    wsptr++;
}


/**********************************************************
 *
 **********************************************************/
void delwhile(void)
{
    if(readwhile()) wsptr--;
}


/**********************************************************
 *
 **********************************************************/
wsentry *readwhile(void)
{
    if(wsptr==startws) {
        error("no active do/for/while/switch");
        return(0);
    }else
    return(wsptr--);
}


/**********************************************************
 *
 **********************************************************/
wsentry *findwhile(void)
{
    wsentry *ptr;

    for(ptr=wsptr;ptr!=startws;) {
        ptr--;
        if(ptr->wstyp!=WSSWITCH) return(ptr);
    }
    error("no active do/for/while");
    return(0);
}


/*********************************************************
 *
 **********************************************************/
wsentry *readswitch(void)
{
    wsentry *ptr;

    if((ptr=readwhile()))
        if(ptr->wstyp==WSSWITCH) return(ptr);
    return(0);
}


/**********************************************************
 *
 **********************************************************/
void addcase(int val)
{
    int lab;

    if(swstp==SWSTSZ)
        error("too many case labels");
    else {
        swstcase[swstp]=val;
        swstlab[swstp++]=lab=getlabel();

        printlabel(lab);         /* gnlabel(lab) */
        underscore();
        nl();
    }
}
