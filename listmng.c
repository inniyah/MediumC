/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   listmng.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/* local functions */
void defstorage(void);

/***********************************************************
 * return ptr to an entry in a given list symbol table
 * or NULL if not found
 * table is searched from top to bottom
 ************************************************************/
entry *findglb(list *root, char *sname)
{
    entry *ptr;

    if(TRACE) fprintf(output,";findglb(%s)\n",sname);

    ptr=root->start;
    while(ptr) {
        if(astreq(sname,ptr->name,NAMEMAX)) return(ptr);
        ptr=ptr->next;
    }
    return(0);
}


/***********************************************************
 * return ptr to an entry in a given statement part
 * or entire list, or NULL if not found
 * table is searched from bottom to top
 ************************************************************/
entry *findloc(list *root, char *sname, int part)
{
    entry *ptr;

    if(TRACE) fprintf(output,";findloc(%s)\n",sname);

    ptr=root->end;
    while(ptr) {
        if(part&&ptr->endstmt) return(NULL);
        if(astreq(sname,ptr->name,NAMEMAX)) return(ptr);
        ptr=ptr->prev;
    }
    return(NULL);
}


/***********************************************************
 * return ptr to a typedef entry in local and/orglobal
 * symbol table or NULL if not found
 ************************************************************/
entry *findtype(char *sname)
{
    entry *p;

    if(TRACE) fprintf(output,";findtype(%s)\n",sname);

    p=loctab->end;
    while(p) {
        if((p->storage==TYPEDEF)&&astreq(sname,p->name,NAMEMAX)) return(p);
        p=p->prev;
    }

    p=glbtab->end;
    while(p) {
        if((p->storage==TYPEDEF)&&astreq(sname,p->name,NAMEMAX)) return(p);
        p=p->prev;
    }

    return(NULL);
}


/***********************************************************
 * return ptr to an entry or NULL if not found
 * search entire loctab and glbtab
 ************************************************************/
entry *find(char *sname)
{
    entry *ptr;

    if(TRACE) fprintf(output,";find(%s)\n",sname);

    if ( (ptr = findloc(loctab, sname,ALL)) ) return(ptr);
    if ( (ptr = findglb(glbtab, sname)) ) return(ptr);
    return(NULL);
}


/***********************************************************
 * remove a table
 * remove the entire table and release the memory
 ************************************************************/
void rmtable(list *root)
{
    entry *p, *prev_p;

    if(TRACE) fprintf(output,";rmtable()\n");

    if(root==NULL) return;
    p=root->end;
    while(p) {
        if((((p->type==UNION)||(p->type==STRUCT))&&(p->ident==IDENT))||
            (p->ident==FUNCTION)||(p->ident==PROTOTYPE)) rmtable(p->further);
        prev_p = p->prev;
        free(p);
        p = prev_p;
    }
    root->entries = 0;
    root->start=root->end = NULL;
}


/***********************************************************
 * mark the begining of a recursive statement
 * return the address
 ************************************************************/
entry *markend(void)
{
    if(TRACE) fprintf(output,";markend()\n");

    loctab->end->endstmt=1;
    return(loctab->end);
}


/***********************************************************
 * trim a (local) table
 * trims the table by removing the entries up to indicated
 ************************************************************/
void trimtable(entry *item)
{
    entry *p, *prev_p;

    if(TRACE) fprintf(output,";trimtable()\n");

    p=loctab->end;
    while(p!=item) {
        if((((p->type==UNION)||(p->type==STRUCT))&&(p->ident==IDENT))||
            (p->ident==FUNCTION)||(p->ident==PROTOTYPE)) rmtable(p->further);
        loctab->entries--;
        prev_p = p->prev;
        free(p);
        p = prev_p;
    }
    loctab->end=item;
    loctab->end->endstmt=0;
    loctab->end->next=NULL;
}


/***********************************************************
 * add entry in local symbol table
 * for static entries generate the storage space
 ************************************************************/
entry *addsymbol(list *root, char *sname, T_storage stor,
T_type typ, T_ident id, unsigned int size)
{
    entry *newptr;

    if(TRACE) fprintf(output,";addsymbol(%s)\n",sname);

    if (  (newptr = find(sname)) ) return(newptr);

    newptr=(entry *)calloc(1,sizeof(entry));

    if(root->start==NULL) {
        root->start=root->end=newptr;
        newptr->next=newptr->prev=NULL;
    }
    else {
        root->end->next=newptr;
        newptr->prev=root->end;
        root->end=newptr;
    }

    strcpy(newptr->name,(stor==LSTATIC ? makename():sname));
    newptr->ident=id;
    newptr->type=typ;
    newptr->storage=stor;
    newptr->further=NULL;
    newptr->size=size;
    newptr->next=NULL;
    root->entries++;

    return(newptr);
}
