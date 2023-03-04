/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   sym.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/***********************************************************
 * process the list in enum declaration
 * identical for global and local declarations
 ***********************************************************/
void doconsts(list *table,T_storage stor)
{
    entry *p;
    char sname[NAMESIZE];
    int num=0;                   /* enums start from 0 */

    while(!match("}")) {
        if(!symname(sname)) error("Illegal symbol name");
        if(find(sname))
            error("Duplicate define");
        else {
            p=addsymbol(table,sname,stor,ENUM,CONST,ENUMSIZE);
            if(match("=")) num=getint();
            p->misc.ivalue=num++;
        }
        if(match(",")) continue;
    }
}


/***************************************************************
 * find size, in bytes, of a list
 ***************************************************************/
int getsize(entry *item, int what)
{
    entry *p;
    int size=0;

    p=item->further->start;

    switch(what) {
        case ENUM:
            size=ENUMSIZE;
            break;

        case STRUCT:             /* size of the entire storage */
            while(p) {
                size+=p->size;
                p=p->next;
            }
            break;

        case UNION:              /* size of the biggest member */
            while(p) {
                if(p->size>size) size=p->size;
                p=p->next;
            }
            break;
    }
    return(size);
}


/*************************************************************
 *	get required array size
what about multidimensional arrays
**************************************************************/
int needsub(void)
{
    int num[1];

    if(match("]")) return(0);
    if(!number(num)) {
        error("Must be a constant");
        num[0]=1;
    }
    if(num[0]<0) {
        error("Negative size illegal");
        num[0]=(-num[0]);
    }
    needbrack("]");
    return(num[0]);
}


/**********************************************************
 *	test if next input string is legal symbol name
 * return true if OK and fetch the name
 ***********************************************************/
int symname(char *sname)
{
    int k;
    char    c;

    blanks();
    sname[0]=0;
    if(!alpha(ch())) return(0);
    k=0;
    while(alnum(ch())) sname[k++]=gch();
    sname[k]=0;
    return(1);
}


/***********************************************************
 * push name back into parser input buffer
 ************************************************************/
void ungetname(char *name)
{
    lptr-=strlen(name);
}


/***********************************************************
 * print error msg
 * move to error.c ?
 ************************************************************/
void multidef(char *sname)
{
    error("already defined");
    comment();
    outstr(sname);
    nl();
}
