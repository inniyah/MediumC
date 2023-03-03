/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   glbsym.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

/* local functions */
void dostrun(list *root, T_storage stor, int what);
void doglbmembers(entry *item,T_storage stor);
void offsglbstruct(entry *item,int what);
void declstun(list *table, entry *ptyp, T_storage stor);
void dotypedef(list *table);

/**********************************************************
*	parse top level declarations of:
* typedef, enum, struct, union, other data types
* and functions
* class is one of: extern, static or public
**********************************************************/
void doglobal(T_storage stor)
{
	char name[NAMESIZE];
	entry *ptyp;

	if(match("typedef"))
		dotypedef(glbtab);
	else if(match("enum"))
		dostrun(glbtab,stor,ENUM);
	else if(match("struct"))
		dostrun(glbtab,stor,STRUCT);
	else if(match("union"))
		dostrun(glbtab,stor,UNION);
	else{
		if(!symname(name)) error("Illegal symbol name");
		if(ptyp=findtype(name))
			dosymbol(glbtab,ptyp,stor,1);
		else
			newfunc(name,INT);							/* assume int function */
	}
}

/*********************************************************
* process global enum/struct/union declaration 
* syntax:
* struct/union id {..};			- type id description only
* struct/union id {..} var; - type id descr and var entry 
* struct/union id var;      - variable entry
* struct/union {..} var;    - variable entry, no type id
*********************************************************/
void dostrun(list *root, T_storage stor, int what)
{
	char sname[NAMESIZE];
	entry *tmp;

	if(!match("{")){																/* struct id .. */
		if(!symname(sname)){
			error("Illegal symbol name");
			return;
		}

		if(match("{")){																/* struct id {..}.. */
			if(findglb(glbtab,sname)) error("Duplicate struct id");
			tmp=addsymbol(glbtab,sname,stor,what,IDENT,0);
			switch(what){
				case ENUM:
					doconsts(glbtab,stor);
					break;
				case STRUCT:
				case UNION:
					doglbmembers(tmp,stor);									/* process members */
					tmp->size=getsize(tmp,what);
					offsglbstruct(tmp,what);
					break;
			}
		}																				

		if(match(";")) return;

		if(!(tmp=findglb(glbtab,sname)))							/* declare struct var */	
			error("Undefined enum, struct or union");
		else
 			switch(what){
				case ENUM:
					dosymbol(root,tmp,stor,0);
					break;
	 			case STRUCT:
	 			case UNION:
	 				declstun(root,tmp,stor);
	 			  break;
				default:
					error("Declaration syntax error");
			}
		return;
	}
	
	tmp=addsymbol(glbtab,makename(),stor,what,IDENT,0);
	switch(what){
		case ENUM:
			doconsts(root,stor);
			dosymbol(root,tmp,stor,0);
		case STRUCT:
		case UNION:
			doglbmembers(tmp,stor);									/* struct/union {..}.. */
			tmp->size=getsize(tmp,what);
			offsglbstruct(tmp,what);
			declstun(root,tmp,stor);					/* declare var */
			break;
	}
}

/*********************************************************
*	process definition of struct and union members
* and add entries to a particular table
* nested enums, structs and unions are allowed
*********************************************************/
void doglbmembers(entry *item,T_storage stor)
{
	entry *ptyp;
	char name[NAMESIZE];
	
	item->further=(list*)calloc(1,sizeof(list));

	while(!match("}")){
		if(match("struct"))
			dostrun(item->further,stor,STRUCT);
		else if(match("union"))
			dostrun(item->further,stor,UNION);
		else if(match("enum"))
			dostrun(item->further,stor,ENUM);
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
* for global struct, adjust down (in bytes) the offset 
*VERIFY
***************************************************************/
void offsglbstruct(entry *item,int what)
{
	entry *p;
	int offset=0;

	if(what==UNION) return;
	
	p=item->further->start;
	while(p){
		p->offset=offset;
		offset+=p->size;
		p=p->next;
	}
}

/*********************************************************
*	declare a global struct/union variable or pointer.
*********************************************************/
void declstun(list *table, entry *ptyp, T_storage stor)
{
	T_ident id;
	char sname[NAMESIZE];
	entry *tmp;
	int size;

	while(1){
		if(match("*"))
			id=POINTER;
		else
			id=VARIABLE;
		if(!symname(sname)) error("Illegal symbol name");
		if(findloc(table,sname,PART)) multidef(sname);

		switch(stor){
			case AUTO:
				if(table==loctab) stkp-=ptyp->size;
				tmp=addsymbol(table,sname,stor,ptyp->ident,id,ptyp->size);
				if(table==loctab) tmp->stkoffs=stkp;
				break;

			case PUBLIC:
			case STATIC:
			case LSTATIC:
				tmp=addsymbol(table,sname,stor,ptyp->ident,id,ptyp->size);
				break;
				
			default:
				error("Ughh, still error in compiler code...");
		}

/*	here we can initiate values
if(match("=")){
}
*/

		tmp->further=ptyp->further;
		if(match(","))	continue;
		if(match(";")) return;
		error("Declaration syntax error");
		junk();
		return;
	}
}

/************************************************************
*	declare variable, array, pointer or function
**************************************************************/
void dosymbol(list *root, entry *ptyp, T_storage stor, int fun)
{
	int size;
	entry *tmp;
	T_ident id;
	char sname[NAMESIZE];

	while(1){
		size=ptyp->size;
		if(match("*"))
			id=POINTER;
		else
			id=VARIABLE;

		if(!symname(sname)) error("Illegal symbol name");
		if(findloc(root,sname,PART)) multidef(sname);
		if(match("[")){					/* what about more dimensions?*/
			size=needsub();
			if(size){
				id=ARRAY;
				size*=ptyp->size;
			}else{
				id=POINTER;
				size=POINTERSIZE; /* size rozmiar ptr czy elementu wskazywanego?*/
			}
		}else
			size=ptyp->size;

		if(fun && (ch()=='(')){
			newfunc(sname,ptyp->type);
			return;
		}

		switch(stor){
			case AUTO:
				if(root==loctab) stkp-=size;
				tmp=addsymbol(root,sname,stor,ptyp->type,id,size);
				if(root==loctab) tmp->stkoffs=stkp;
				break;
	
			default:
				tmp=addsymbol(root,sname,stor,ptyp->type,id,size);
				break;
		}	

/*	here we can initiate values
if(match("=")){
}
*/	
		if(match(","))	continue;
		if(match(";")) return;
		error("declaration syntax error");
		junk();
		return;
	}
}

/********************************************************
* process typedef keyword 
*WRITE UP the code
********************************************************/
void dotypedef(list *table)
{
	error("Not implemented yet");
}
