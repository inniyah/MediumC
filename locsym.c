/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   locsym.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

void dolocmembers(entry *item,T_storage stor);
/*
void offslocstruct(entry *item);
void offslocunion(entry *item);
*/
void offsetloc1(entry *item, int what);
void dostrun1(list *root, T_storage stor, int what);

/************************************************************
* parse local level declarations.
* return 1 on successful declaration of a var
************************************************************/
int dolocal(T_storage stclass)
{
	char name[NAMESIZE];
	entry *ptyp;

	blanks();
	if(match("typedef"))
		dotypedef(loctab);
	else if(match("enum"))
		dostrun1(loctab,stclass,ENUM);
	else if(match("struct"))
		dostrun1(loctab,stclass,STRUCT);
	else if(match("union"))
		dostrun1(loctab,stclass,UNION);
	else{
		symname(name);
		if(ptyp=findtype(name)){
			dosymbol(loctab,ptyp,stclass,0);
			return(1);
		}
		ungetname(name);
		return(0);
	}
}

/*********************************************************
*	process local definition of struct and union members
* and add entries to a particular table
* nested enums, structs and unions are allowed
*********************************************************/
void dolocmembers(entry *item,T_storage stor)
{
	entry *ptyp;
	char name[NAMESIZE];

	item->further=(list*)calloc(1,sizeof(list));

	while(!match("}")){
		if(match("struct"))
			dostrun1(item->further,stor,STRUCT);
		else if(match("union"))
			dostrun1(item->further,stor,UNION);
		else if(match("enum"))
			dostrun1(item->further,stor,ENUM);
		else{
			if(!symname(name)) error("Illegal symbol name");
			if(ptyp=findtype(name)){
				dosymbol(item->further,ptyp,stor,0);
			}else
				error("Declaration syntax error");
		}
	}
}

/***************************************************************
* for local struct/union adjust up (in bytes) the offset
***************************************************************/
void offsetloc1(entry *item, int what)
{
	entry *p;
	int offset=0;
	int size;

	if(what==UNION) return;

	p=item->further->end;
	while(p){
		p->offset=offset;
		offset+=p->size;
		p=p->prev;
	}
}

/*********************************************************
* process local enum/struct/union declaration 
* syntax:
* struct/union id {..};			- type id description only
* struct/union id {..} var; - type id descr. and var. entry 
* struct/union id var;      - variable entry
* struct/union {..} var;    - variable entry, no type id
*********************************************************/
void dostrun1(list *root, T_storage stor, int what)
{
	char sname[NAMESIZE];
	entry *tmp;

	if(!match("{")){																/* struct id .. */
		if(!symname(sname)){
			error("Illegal symbol name");
			return;
		}

		if(match("{")){																/* struct id {..}.. */
			if(findloc(loctab,sname,PART)) error("Duplicate struct id");
			tmp=addsymbol(loctab,sname,stor,what,IDENT,0);
			switch(what){
				case ENUM:
					doconsts(loctab,stor);
					break;
				case STRUCT:
				case UNION:
					dolocmembers(tmp,stor);											/* process members */
					tmp->size=getsize(tmp,what);
					offsetloc1(tmp,what);
					break;
			}
		}											

		if(match(";")) return;

		if(!(tmp=find(sname)))							/* declare struct var */
			error("Undefined enum, struct or union");
		else
 			switch(what){
 				case ENUM:	dosymbol(root,tmp,stor,0);
 										break;
	 			case STRUCT:
	 			case UNION:	declstun(root,tmp,stor);
	 								  break;
				default:	error("Declaration syntax error");
			}
		return;
	}

	tmp=addsymbol(loctab,makename(),stor,what,IDENT,0);
	switch(what){
		case ENUM:
			doconsts(root,stor);
			dosymbol(root,tmp,stor,0);
		case STRUCT:
		case UNION:
			dolocmembers(tmp,stor);									/* struct/union {..}.. */
			tmp->size=getsize(tmp,what);
			offsetloc1(tmp,what);
			declstun(root,tmp,stor);					/* declare var */
			break;
	}
}

/***************************************************************
* adjust offset, in bytes, from local struct end up
VERIFY
***************************************************************/
void offslocstruct(entry *item)
{
	entry *p;
	int offset=0;
	int size;

	p=item->further->end;
	while(p){
		p->offset=offset;
		offset+=p->size;
		p=p->prev;
	}
}

/***************************************************************
* adjust offset, in bytes, from local union end up
VERIFY size
***************************************************************/
void offslocunion(entry *item)
{
	entry *p;
	int size;

	p=item->further->end;
	while(p){
		p->offset=size-p->size;
		p=p->prev;
	}
}
