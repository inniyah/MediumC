/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   init.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

int nxtlab=0;				/* asm label generator storage */ 

/*********************************************************
* return next available internal label number
**********************************************************/
int getlabel(void)
{
	return(nxtlab++);
}

/*********************************************************
* return next available internal label name
**********************************************************/
char *makename(void)
{
	static char name[NAMESIZE];
	snprintf(name, NAMESIZE, "%d", getlabel());
	return(name);
}


/*********************************************************
*	print specified number as label
**********************************************************/
void printlabel(int label)
{
	underscore();
	outdec(label);
}

/**********************************************************
*	gnlabel - generate numeric label
**********************************************************/
void gnlabel(int nlab)
{
	printlabel(nlab);
	underscore();
	nl();
}

/*********************************************************
* print char to output file
*********************************************************/
char outbyte(char c)
{
	if(c==0) return(0);
	fputc(c,output);
	return(c);
}

/*********************************************************
* print str to output file
use pointers instead..
*********************************************************/
void outstr(char ptr[])
{
	int	k;

	k=0;
	while(outbyte(ptr[k++]));
}

/*********************************************************
* print tab to output file
* use #define
*********************************************************/
void tab(void)
{
	outbyte(TAB);
}

/*********************************************************
* print new line to output filr
* use #define
**********************************************************/
void nl(void)
{
	outbyte(EOL);
}

/*********************************************************
* print tab, str and nl to output file
*********************************************************/
void ol(char *str)
{
	ot(str);
	nl();
}

/*********************************************************
* print underscore to output file
* use #define
**********************************************************/
void prefix(void)
{
/*	outbyte('_');*/
}

/*********************************************************
* print tab and str to output file
*********************************************************/
void ot(char *str)
{
	tab();
	outstr(str);
}

/*********************************************************
* print int number to output file
*********************************************************/
void outdec(int number)
{

	char str[10];

	sprintf(str,"%d",number);
	outstr(str);
}
		
/*********************************************************
* store item in VM memory
*********************************************************/
void store(entry *item)
{
	if(TRACE) fprintf(output,";store(%s)\n",item->name);

	if(item->lval1==0)				/* ident==static */
		vmputmem(item->name);
	else
		vmputstk(item);

}

/*********************************************************
* get rvalue item into primary register
*********************************************************/
void rvalue(entry *item)
{
	if(TRACE) fprintf(output,";rvalue(%s)\n",item->name);

	if((item->ident!=CONST) &&(item->lval1==0)) /* static */
		vmgetmem(item);
	else
		vmindirect(item);
}

/*********************************************************
*
*********************************************************/
void test(int label, int ft)
{
	needbrack("(");
	expression(TRUE);
	needbrack(")");
	vmtestjump(label,ft);
}
