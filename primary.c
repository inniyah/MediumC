/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   primary.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include <ctype.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

/*local functions */
void symsizeof(void);
int constant(int val[]);
int qstr(int val[]);
int spechar(void);

/*********************************************************
* return size of symbol
*********************************************************/
void symsizeof(void)				/* int symsizeof() ? */
{
	char sname[NAMESIZE];
	entry *ptr;
	int k;

	needbrack("(");
	immed();
	if(match("int"))
		outdec(INTSIZE);
	else if(match("char"))
		outdec(CHARSIZE);
	else if(match("float"))
		outdec(FLOATSIZE);
	else if(symname(sname)){
		if((ptr=findloc(loctab,sname,ALL))||(ptr=findglb(glbtab,sname))){
			if(ptr->storage==LSTATIC) error("sizeof local static");
			k=ptr->size;
			if((ptr->type==INT) || (ptr->ident==POINTER)) k*=INTSIZE;
			outdec(k);
		}
		else{
			error("sizeof undeclared variable");
			outdec(0);
		}
	}
	else
		error("sizeof only on type or variable");
	needbrack(")");
	nl();
}

/*********************************************************
* here we can find struct or union, VERIFY
* get primary value
* ONE of the most important functions!!
*********************************************************/
int primary(entry *item)
{
	entry *ptr,*sptr;
	char sname[NAMESIZE];
	int	num[1];
	int	offset,k;


	if(match("(")){
		k=heir1(item);
		needbrack(")");
		return(k);
	}

	if(match("sizeof")){
		symsizeof();
		/*item=NULL; ?????????? */
		item->lval0=0;
		item->lval1=0;
		item->lval2=0;
		return(0);
 	}

/******************** local *******************/
	if(symname(sname)){
		if((ptr=findloc(loctab,sname,ALL))){
*item=*ptr;
			switch(ptr->ident){
 			case CONST:
				immed();
				outdec(item->misc.ivalue);
				nl();
				item->lval0=0;
				item->lval1=0;
				item->lval2=0;
				return(0);
				
			case VARIABLE:
				vmgetloc(item);
				item->lval1=item->type;
				item->lval2=0;
				return(1);
				
			case POINTER:
				vmgetloc(item);
				item->lval1=INT;
				item->lval2=item->type;
				return(1);
				
			case ARRAY:
				vmgetloc(item);
				item->lval2=item->type;
				item->lval2=0;
				return(0);
			}
		}

/********** global ***********/
		if(ptr=findglb(glbtab,sname)){
			*item=*ptr;
				switch(ptr->ident){
				case CONST:
					immed();
					outdec(item->misc.ivalue);
					nl();
					item->lval0=0;
					item->lval1=0;
					item->lval2=0;
					return(0);

				case VARIABLE:
					item->lval0=0;
					item->lval1=0;
					item->lval2=0;
					return(1);
				
				case POINTER:
					item->lval0=0;
					item->lval1=0;
					item->lval2=item->type;
					return(1);
									
				case ARRAY:
					immed();
					prefix();
					outstr(item->name);
        	underscore();
					nl();
			item->lval1=item->lval2=item->type;
			item->lval2=0;
					return(0);
		
				default:											/* ie FUNCTION */
					blanks();
					if(ch()!='(') error("undeclared variable");
					ptr=addsymbol(glbtab,sname,PUBLIC,INT,FUNCTION,0);
					*item=*ptr;
					item->lval0=0;
					item->lval1=0;
					return(0);
			}
		}
	}

	if(constant(num)){
		item=NULL;
		return(0);
	}else{
		error("invalid expression");
		immed();
		outdec(0);
		nl();
		junk();
		return(0);
	}
}

/************************************************************
*	true if val1->int pointer or int array and
* val2 not pointer or array
*************************************************************/
int dbltest(entry *val1, entry *val2)
{
	if(val1->lval1==NULL) return(FALSE);
	if(val1->lval2!=INT) return(FALSE);
	if(val2->lval2)	return(FALSE);
	return(TRUE);

/* or..
	if(val1->type!=INT) return(FALSE);
	if((val2->ident==POINTER)||(val2->ident==ARRAY))	return(FALSE);
*/
}

/*************************************************************
*	determine type of binary operation ???
**************************************************************/
void result(entry *val, entry *val2)
{
	if(val->lval2 && val2->lval2)	val->lval2=0;
	else
	  if(val2->lval2){
			val->lval0=val2->lval0;
			val->lval1=val2->lval1;
			val->lval2=val2->lval2;
	}
}

/*********************************************************
* better use library functions
*********************************************************/		
int constant(int val[])
{
	if(number(val))
		immed();
	else if(pstr(val))
		immed();
	else if(qstr(val)){
		immed();

/*
fix string labels
output only offset
   printlabel (litlab);
   underscore();
   outbyte ('+');
*/
	}
	else
		return(0);

	outdec(val[0]);
	nl();
	return(1);
}

/**********************************************************
* use some standard library function
* get numeric value form input buffer
**********************************************************/
int number(int val[])
{
	int	k, minus, base;
	char	c;

	k=minus=1;
	while(k){
		k=0;
		if(match("+")) k=1;
		if(match("-")){
			minus=(-minus);
			k=1;
		}
	}
	if(!isdigit(c=ch())) return(0);
	if(match("0x") || match("0X"))
		while(isdigit(c=ch()) ||
		       (c>='a' && c<='f') ||
		       (c>='A' && c<='F')) {
			inchar();
			k=k*16+(isdigit(c) ? (c-'0'):((c & 07)+9));
		}
	else{
		base=(c=='0') ? 8:10;
		while(isdigit(ch())){
			c=inchar();
			k=k*base+(c-'0');
		}
	}
	if(minus<0)	k=(-k);
	val[0]=k;
	return(1);
}


/**********************************************************
* ???
* use some standard library function
**********************************************************/
int getint(void)
{
	int	k, minus, base;
	char	c;

	k=minus=1;
	while(k){
		k=0;
		if(match("+")) k=1;
		if(match("-")){
			minus=(-minus);
			k=1;
		}
	}
	if(!isdigit(c=ch())) return(0);		/* error */
	if(match("0x")||match("0X"))
		while(isdigit(c=ch())||(c>='a' && c<='f')||(c>='A' && c<='F')){
			inchar();
			k=k*16+(isdigit(c) ? (c-'0'):((c & 07)+9));
		}
	else{
		base=(c=='0') ? 8:10;
		while(isdigit(ch())){
			c=inchar();
			k=k*base+(c-'0');
		}
	}
	if(minus<0)	k=(-k);
	return(k);
}

/**********************************************************
* ??? 
**********************************************************/
int pstr(int val[])
{
	int	k;
	char	c;

	k=0;
	if(!match("'"))	return(0);
	while((c=gch()) !=39){
		c=(c=='\\') ? spechar():c;
		k=(k & 255)*256+(c & 255);
	}
	val[0]=k;
	return(1);
}


/********************************************************
* ???
********************************************************/
int qstr(int val[])
{
	char	c;

	if(!match("\""))	return(0);
	val[0]=litptr;
	while(ch()!='"'){
		if(ch()==0)	break;
		if(litptr>=LITMAX){
			error("string space exhausted");
			while(!match("\""))
				if(gch()==0) break;
			return(1);
		}
		c=gch();
		litq[litptr++]=(c=='\\') ? spechar():c;
	}
	gch();
	litq[litptr++]=0;
	return(1);
}

/***********************************************************
*	decode special characters (preceeded by back slashes)
************************************************************/
int spechar(void)
{
	char c;

	c=ch();

	if(c=='n') c=EOL;
	else if(c=='t') c=TAB;
	else if(c=='r') c=CR;
	else if(c=='f') c=FF;
	else if(c=='b') c=BS;
	else if(c=='0') c=EOS;
	else if(c=='e') c=ESC;		/* one extra useful value */
	else if(c==EOS) return;		/* return what ? */
	gch();
	return(c);
}
