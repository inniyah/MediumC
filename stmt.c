/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   stmt.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

int lastst=0;
/*local functions */
int parsestdecl(void);
void stst(char *fname);
void compound(char *fname,int func);
void doif(char *fname);
void dowhile(char *fname);
void dodo(char *fname);
void dofor(char *fname);
void doswitch(char *fname);
void docase(void);
void dodefault(void);
void doreturn(char *fname);
void dobreak(void);
void docont(void);
void dumpsw(wsentry *ws);

/*************************************************************
*	statement parser
*
*	called whenever syntax requires a statement.  this routine
*	performs that statement and returns a number telling which one
*
*	'func' is true if we require a "function_statement", which
*	must be compound, and must contain "statement_list" (even if
*	"declaration_list" is omitted)
**************************************************************/
int statement(char *fname, int func)
{
	if((ch()==0) & feof(input))	return(0);
	lastst=0;
	if(func)
		if(match("{")){
			compound(fname,TRUE);
			return(lastst);
		}else{
			error("function requires compound statement");
	}
	if(match("{"))
		compound(fname,FALSE);
	else
		stst(fname);
	return(lastst);
}

/***********************************************************
*	parse local statement declarations
* return 1 for declaration
************************************************************/
int parsestdecl(void)
{
	blanks();

	if(match("register"))
		dolocal(DEFAUTO);
	else if(match("auto"))
		dolocal(DEFAUTO);
	else if(match("static"))
		dolocal(LSTATIC);
	else if(dolocal(AUTO));
	else
		return(0);						/* no more declarations */
	return(1);
}

/************************************************************
*	non-declaration statement
************************************************************/
void stst(char *fname)
{
	if(match("if")){
		doif(fname);
		lastst=STIF;
	}else if(match("while")){
		dowhile(fname);
		lastst=STWHILE;
	}else if(match("switch")){
		doswitch(fname);
		lastst=STSWITCH;
	}else if(match("do")){
		dodo(fname);
		issemicolon();
		lastst=STDO;
	}else if(match("for")){
		dofor(fname);
		lastst=STFOR;
	}else if(match("return")){
		doreturn(fname);
		issemicolon();
		lastst=STRETURN;
	}else if(match("break")){
		dobreak();
		issemicolon();
		lastst=STBREAK;
	}else if(match("continue")){
		docont();
		issemicolon();
		lastst=STCONT;
	}else if(match(";"))
		;
	else if(match("case")){
		docase();
		lastst=statement(fname,FALSE);
	}else if(match("default")){
		dodefault();
		lastst=statement(fname,FALSE);
	}else if(match("#asm")){
		doasm();
		lastst=STASM;
	}else if(match("{")){
		entry *flev;

		flev=markend();
		compound(fname,FALSE);
		trimtable(flev);
	}
	else{
		expression(TRUE);

/*		if (match (":")) {
			dolabel();
			lastst=statement(fname,FALSE);
		}else{
*/		
issemicolon();

			lastst=STEXP;
/*		}
*/	}
}

/************************************************************
*	compound statement
*	allow any number of statements to fall between "{" and "}"
*
*	'func' is true if we are in a "function_statement", which
*	must contain "statement_list"
func is not used..
************************************************************/
void compound(char *fname,int func)
{
	int	decls=TRUE;

	int stkptr=stkp;										/* temp stack ptr */

	ncmp++;
	while(!match("}")){
		if(feof(input)) return;
		if(decls){ 												/* decl local variables */
			if(!parsestdecl()){
				vmmodstk(2*stkp-stkptr);				/* alloc AUTO variables */
				decls=FALSE;
			}
		}else
			stst(fname);										/* process instructions */
	}

	if(decls){
		vmmodstk(2*stkp-stkptr);						/* no statement follows */
	}
	ncmp--;
}

/************************************************************
*	"if" statement
************************************************************/
void doif(char *fname)
{
	int	fstkp, flab1, flab2;
	entry *flev;

	flev=markend();
	fstkp=stkp;
	flab1=getlabel();						/* next available label */
	test(flab1,FALSE);					/* jump on false */
	statement(fname,FALSE);			/* statement on true */
	stkp=vmmodstk(fstkp);
	trimtable(flev);
	if(!match("else")){
		gnlabel(flab1);						/* signal else statement */
		return;
	}
	vmjump(flab2=getlabel());			/* jump to exit point */
	gnlabel(flab1);							/* signal */ 
	statement(fname,FALSE);
	stkp=vmmodstk(fstkp);
	trimtable(flev);
	gnlabel(flab2);							/* signal exit point */
}

/************************************************************
*	"while" statement
************************************************************/
void dowhile(char *fname)
{
	wsentry ws;

	ws.wssym=markend();
	ws.wssp=stkp;
	ws.wstyp=WSWHILE;
	ws.wstest=getlabel();
	ws.wsexit=getlabel();
	addwhile(ws);
	gnlabel(ws.wstest);						/* signal start of test expr */
	test(ws.wsexit,FALSE);				/* exit on false */
	statement(fname,FALSE);				/* body */
	vmjump(ws.wstest);							/* jump to test expr */
	gnlabel(ws.wsexit);						/* signal exit point */
	trimtable(ws.wssym);
	stkp=vmmodstk(ws.wssp);
	delwhile();
}

/************************************************************
*	"do" statement
************************************************************/
void dodo(char *fname)
{
	wsentry ws;

	ws.wssym=markend();
	ws.wssp=stkp;
	ws.wstyp=WSDO;
	ws.wsbody=getlabel();
	ws.wstest=getlabel();
	ws.wsexit=getlabel();
	addwhile(ws);
	gnlabel(ws.wsbody);					/* signal start of body */
	statement(fname,FALSE);			/* body */
	if(!match("while")){
		error("missing while");
		return;
	}
	gnlabel(ws.wstest);					/* signal start of test expr */
	test(ws.wsbody,TRUE);				/* jump back to body start */
	gnlabel(ws.wsexit);					/* signal exit point */
	trimtable(ws.wssym);
	stkp=vmmodstk(ws.wssp);
	delwhile();
}

/************************************************************
*	"for" statement
************************************************************/
void dofor(char *fname)
{
	wsentry ws,*pws;

	ws.wssym=markend();
	ws.wssp=stkp;
	ws.wstyp=WSFOR;
	ws.wstest=getlabel();
	ws.wsincr=getlabel();
	ws.wsbody=getlabel();
	ws.wsexit=getlabel();
	addwhile(ws);
	pws=readwhile();
	needbrack("(");
	if(!match(";")){
		expression(TRUE);								/* initial value */
		issemicolon();
	}
	gnlabel(pws->wstest);							/* signal start of test expr */
	if(!match(";")){
		expression(TRUE);
		vmtestjump(pws->wsbody,TRUE);			/* if OK jump to body */
		vmjump(pws->wsexit);							/* jump to exit point */
		issemicolon();
	}else
		pws->wstest=pws->wsbody;
	  gnlabel(pws->wsincr);						/* signal start of increment expr */
		if(!match(")")){
			expression(TRUE);
			needbrack(")");
			vmjump(pws->wstest);						/* jump to test point */
		}else
			pws->wsincr=pws->wstest;
		gnlabel(pws->wsbody);						/* signal start of body */
		statement(fname,FALSE);
		vmjump(pws->wsincr);							/* jump to increment 
		gnlabel(pws->wsexit);						/* signal exit point */
		trimtable(pws->wssym);
		stkp=vmmodstk(pws->wssp);
		delwhile();
}

/************************************************************
*	"switch" statement
* VERIFY
************************************************************/
void doswitch(char *fname)
{
	wsentry ws, *ptr;

	ws.wssym=markend();
	ws.wssp=stkp;
	ws.wstyp=WSSWITCH;
	ws.wscasep=swstp;
	ws.wstab=getlabel();
	ws.wsdef=ws.wsexit=getlabel();
	addwhile(ws);

#ifdef OLD_SWITCH
	immed();
	printlabel(ws.wstab);   					/* gnlabel */
  underscore();  /* added 2/12/95 */
	nl();
	gpush();
#endif

	needbrack("(");
	expression(TRUE);											/* switch expression */
	needbrack(")");

/* stkp = stkp + intsize(); */  /* '?case' will adjust the stack */
/* new code generation */

	vmjump(ws.wstab);

/* end of new code generation */

	statement(fname,FALSE);
	ptr=readswitch();
	vmjump(ptr->wsexit);
	dumpsw(ptr);
	gnlabel(ptr->wsexit);								/* signal exit point */ 
	trimtable(ptr->wssym);
	stkp=vmmodstk(ptr->wssp);
	swstp=ptr->wscasep;
	delwhile();
}

/************************************************************
*	"case" label
************************************************************/
void docase(void)
{
	int	val;

	val=0;
	if(readswitch()){
		if(!number(&val))	if(!pstr(&val))	error("bad case label");
		addcase(val);
		if(!match(":"))	error("missing colon");
	}else
		error("no active switch");
}

/************************************************************
*	"default" label
************************************************************/
void dodefault(void)
{
	wsentry *ptr;

	if((ptr=readswitch())){
		ptr->wsdef=getlabel();
		gnlabel(ptr->wsdef);
		if(!match(":"))	error("missing colon");
	}else
		error("no active switch");
}

/************************************************************
*	"return" statement
************************************************************/
void doreturn(char *fname)
{
entry *ptr;

	ptr=findglb(glbtab,fname);
	if(endst()==0){
		if(ptr->type==VOID) error("function is declared as void");
		expression(TRUE);
	}else
		if(ptr->type!=VOID) error("function should return a value");
	vmjump(fexitlab);
}

/************************************************************
*	"break" statement
************************************************************/
void dobreak(void)
{
	wsentry *ptr;

	if((ptr=readwhile())==0) return;
	vmmodstk(ptr->wssp);
	vmjump(ptr->wsexit);
}

/************************************************************
*	"continue" statement
************************************************************/
void docont(void)
{
	wsentry *ptr;

	if((ptr=findwhile())==0) return;
	vmmodstk(ptr->wssp);
	if(ptr->wstyp==WSFOR)
		vmjump(ptr->wsincr);
	else
		vmjump(ptr->wstest);
}

/* shouldn't goto be added?? */

/************************************************************
*	dump switch table
*VERIFY
************************************************************/
void dumpsw(wsentry *ws)
{
	int	i,j;

 /*	gdata ();  */
	gnlabel(ws->wstab);
	if(ws->wscasep!=swstp){
		j=ws->wscasep;
		while(j<swstp){
			i=4;
			while(i--){
        outcase(CASE,swstcase[j],swstlab[j]);
        j++;
				if((i==0)|(j>=swstp))	break;
			}
		}
	}
	outcase(DEFAULT,0,ws->wsdef);
    /*	gtext ();  */
}
