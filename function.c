/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   function.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

int argstk;					/* offset to stack */

/* local functions */
void getstmtlist(entry *item);
void chkstmtlist(entry *item);
void addfunc(entry *item);
void getarg(list *table,entry *ptyp);

/************************************************************
*	begin a function,	called from "parse"
* this routine makes a named function or adds a new function
* declaration must conform to ANSI
VERIFY function type and verify with prototype
************************************************************/
void newfunc(char *sname, T_type typ)
{
	entry *fptr,*ptr;
	char fname[NAMESIZE],name[NAMESIZE];

	if(sname==NULL){
		if(!symname(fname)){												/* check for valid name */
			error("illegal function or declaration");
			return;
		}
	}else
		strcpy(fname,sname);

	if((fptr=findglb(glbtab,fname))){			/* check if already defined */
		if(fptr->ident!=PROTOTYPE)
			multidef(fname);									/* error */
		else{
			chkstmtlist(fptr);								/* verify with prototype */
			addfunc(fptr);										/* add function body */
		}	
	}else{
		fptr=addsymbol(glbtab,fname,PUBLIC,typ,PROTOTYPE,0);
		getstmtlist(fptr);									/* process statement list */

		if(match(";"))
			rmtable(loctab);									/* if prototype kill loctab */
		else{
			fptr->ident=FUNCTION;
			addfunc(fptr);										/* add function body */
		}
	}
}

/**********************************************************
* process function statement list
*	called from newfunc with ptr to function entry.
* this routine adds each named argument in function entry
***********************************************************/
void getstmtlist(entry *fptr)
{
	entry *ptr,*ptyp,*p;
	int argtop;
	char typname[NAMESIZE];

	if(!match("("))	error("missing open paren");

	fptr->further=(list *)calloc(1,sizeof(list));	/* init params table */

	argstk=0;
	if(match("void")){
		if(!match(")")) error("declaration syntax error");
	}else
		while(!match(")")){ 
			match("register");								/* ignore register keyword */
			symname(typname);
			if(ptyp=findtype(typname))
				getarg(fptr->further,ptyp);
			else
				error("unknown typedef");			
			
			if(match(",")){
				blanks();
				if(ch()==')') error("declaration syntax error");
			}
		}

	argtop=argstk;													/* now balance stack */
	p=fptr->further->start;
	ptr=loctab->start;
	while(ptr){
		ptr->offset=argtop-(ptr->offset)-1;
		p->offset=ptr->offset;
		argstk-=ptyp->size;
		ptr=ptr->next;
		p=ptyp->next;
	}
}

/**********************************************************
*	verify function statement list 
*	called from "newfunc" with ptr to function prototype.
* this routine verifies each function entry with prototype.
************************************************************/
void chkstmtlist(entry *fptr)
{
	entry *ptr,*ptyp;
	char typname[NAMESIZE];

if(!match("("))	error("missing open paren");
if(match("void"))
	if(!match(")")) error("declaration syntax error");


	ptr=fptr->further->start;				/* 1st arg in protytype */

	while(!match(")")){							/* till closing bracket */
		if(NULL==ptr){
			error("argnum1");						/* no more params */
			while(!match(")")) lptr++;	/* skip statement list */
			return;
		}

		match("register");								/* ignore register keyword */
		symname(typname);									/* arg type */
		if(ptyp=findtype(typname))
			verarg(ptr,ptyp);								/* proto, arg type */
		else
			error("unknown typedef");			

		if(match(",")){
			blanks();
			if(ch()==')') error("declaration syntax error");
		}
		ptr=ptr->next;	
	}
	if(ptr){
	 error("argnum2");
	}
}

/**********************************************************
*	declare argument from statement list
*	called from newfunc.
* this routine adds each named argument in function entry
***********************************************************/
void getarg(list *table,entry *ptyp)
{
	int	id,legalname;
	char name[NAMESIZE];
	entry *p;

	if(match("*"))
		id=POINTER;
	else
		id=VARIABLE;
	if(!(legalname=symname(name))) error("Illegal symbol name");

	if(match("[")){
		while(inchar()!=']')
			if(endst()) break;
		id=POINTER;
	}
	if(legalname){
		if(findloc(loctab,name,ALL))
			multidef(name);
		else{
			p=addsymbol(loctab,name,AUTO,ptyp->type,PUBLIC,0);	/* add to local global */
			p->size=ptyp->size;
			p->offset=argstk;
			p=addsymbol(table,name,AUTO,ptyp->type,PUBLIC,0);	/* add to a further in global */
			p->size=ptyp->size;
			p->offset=argstk;
			argstk+=ptyp->size;														/* for now assume int size */
		}
	}
}

/**********************************************************
*	verify argument from func statement list vs func prototype
*	called from "newfunc", this routine also adds to loctab each
* argument from function statement list
************************************************************/
verarg(entry *ptr,entry *ptyp)
{
	int	id;
	char name[NAMESIZE];
	entry *p;

	if(NULL==ptr) return;

	if(match("*"))
		id=POINTER;
	else
		id=VARIABLE;
	if(!symname(name)) error("Illegal symbol name");			/* function argument */
	if(match("[")){
		while(inchar()!=']')
			if(endst()) break;
		id=POINTER;
	}
	while(1){
		if(id!=ptr->ident) break;
		if(ptyp->type!=ptr->type) break;
		p=addsymbol(loctab,name,AUTO,ptyp->type,PUBLIC,0);	/* add to local global */
		p->size=ptyp->size;
		p->offset=ptr->offset;
		return;
	}
	error("declaration syntax error");
}

/**********************************************************
*	add function body
*	called from newfunc.
* this routine processes the function statement
************************************************************/
void addfunc(entry *item)
{
	fexitlab=getlabel();							/* end of function label */
	asprologue(item->name);							/* begin code generation */
	prefix();
	outstr(item->name);
	underscore();
	nl();

	stkp=0;
	statement(item->name,TRUE);				/* function statement */
	printlabel(fexitlab);
	underscore();
	nl();
	vmmodstk(0);												/* clear local variables */
	vmret();														/* final return */

	stkp=0;
	if(TRACE) debuglist(loctab,item->name);
	rmtable(loctab);
}

/*****************************************************************
*	perform a function call
*
*	called from "heir11", this routine will either call the named
*	function, or if the supplied ptr is zero, will call the contents
*	of stack
*****************************************************************/
void callfunc(char *fname)
{
	entry *ptr;
	int	id,nargs,argsize;

	nargs=argsize=0;
	blanks();
	if(fname==0){
		vmpush(NULL);   /*will call stack, what type to push ???*/
	}

	ptr=findglb(glbtab,fname);						/* find function */
	id=ptr->ident;
	ptr=ptr->further->start;

	while(!match(")")){										/* till closing bracket */
		if(endst())	break;
		expression(FALSE);									/* evaluate param */
		if(fname==0) vmswapstk();
		vmpush(ptr);												/* push param */
		argsize+=ptr->size;
		nargs++;
		ptr=ptr->next;

		if(match(",")){
			blanks();
			if(ch()==')') error("declaration syntax error");
		}
	}

	if(ptr && (id==PROTOTYPE)) error("wrong number of arguments in call");
	if(aflag)	asargs(nargs);					 		/* indicate number of args */ 

	if(fname)															/* call function */
		vmcall(fname);
	else
		vmcallstk();
	stkp=vmmodstk(stkp+argsize);						/* after call clean stack */
}
