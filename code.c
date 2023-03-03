/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   code.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

/***********************************************************
* it would be better co combine all functions into a structure
* and append selectively the code to the asm output
* this is a part of speed/size optimisation
************************************************************/ 
extern int M_lib;   /* see if we have to load MULT library */
extern int S_lib;   /* see if we need shift library */
extern int G_lib;   /* see if we need comp library */


/* Assembler specific functions */
/*********************************************************
* put initial asm header
*********************************************************/
void asheader(void)
{
	char str[80];
	
	sprintf(str,"\tinclude '%s\\c0s'",LIBDIR);
	comment();
	outstr(VERSION);
	version();
	nl();

	outstr(str);
	nl();
}

/**********************************************************
* print any assembler stuff needed after all code
***********************************************************/
void astrailer(void)
{
	ol("end");
}

/**********************************************************
* output function prologue
***********************************************************/
void asprologue(char *fname)
{
	if(TRACE) fprintf(output,";prologue(%s)\n",fname);
	outstr(";**********************************************************\n");
	outstr(";* function:  ");
  outstr(fname);
	outstr("\n;**********************************************************\n");
}

/*********************************************************
* text (code) segment
*********************************************************/
void astext(void)
{
	if(TRACE) fprintf(output,";gtext()\n");
	ol("\n; **************code segment cseg*******************");
	ol("\n\torg\t_CSEG\n");
}

/*********************************************************
* data segment
**********************************************************/
void asdata(void)
{
	char str[80];
	
	if(TRACE) fprintf(output,";gdata()\n");
  if(M_lib==TRUE){
		outstr("\n;*******need mult/div standard library********\n");
		sprintf(str,"\n\tinclude '%s\\mlibpic.h'\n",LIBDIR);
    ol(str);
  }
  if(S_lib==TRUE){
    outstr("\n;*******need shift/sub standard library********\n");
		sprintf(str,"\n\tinclude '%s\\slibpic.h'\n",LIBDIR);
    ol(str);
  }
	if(G_lib==TRUE){
    outstr("\n;*******need compare standard library********\n");
		sprintf(str,"\n\tinclude '%s\\glibpic.h'\n",LIBDIR);
 		ol(str);
	}
	ol("\n\n; **************data segment dseg*******************");
	ol("\n  org _DSEG\n");
}


/***********************************************************
* multiply the primary register by INTSIZE
?????????
************************************************************/
void gaslint(void)
{
	if(TRACE) fprintf(output,";gaslint()\n");
	/* ol ("dad\th"); */
}

/***********************************************************
* divide the primary register by INTSIZE
????????
************************************************************/
void gasrint(void)
{
	if(TRACE) fprintf(output,";gasrint()\n");
	return;

#ifdef INTEGERS
	gpush();        /* push primary in prep for gasr */
	immed();
	outdec(1);
	nl();
	gasr();  /* divide by two */
#endif
}

/* this is a real hack */
/* output inline code for case statement */

/***********************************************************
*
************************************************************/
void outcase(int typ, int value, int label)
{
  char buffer[50];

  if(TRACE) fprintf(output,";outcase()\n");
  if(typ==CASE){
    sprintf(buffer,"cje\t_primary,#%d,_%d_",value,label);
    ol(buffer);
   }else
     if(typ==DEFAULT){
     	sprintf(buffer,"jmp\t_%d_",label);
      ol(buffer);
    }
    else
/*      outstr(ERRMSG)*/;
}

/***********************************************************
* Squirrel away argument count in a register that modstk
* doesn't touch.
***********************************************************/
void asargs(int d)
{
	if(TRACE) fprintf(output,";gnargs(%d)\n",d);
	ot(";;(# args passed) mov W, #");
	outdec(d);
	nl();
}

/**********************************************************
*	dump the literal pool
**********************************************************/
void asdumplits(void)
{
	int	j, k;
  char ch;

	if(litptr==0)	return;

  outstr("\n; Begin global string table\n");
  outstr("\n_string_table_\n");     /* add a new special label for strings */
  ol("jmp PC+W");
	printlabel(litlab);
	underscore();
	k=0;
	while(k<litptr){
  	ot("retw ");
    outbyte('\'');
		j=255;
		while(j--){
      ch=litq[k++] & 127;
			if((j==0) || (k>=litptr) || (ch==0)){
      	outbyte('\'');
				nl();
        ot("retw 0");
        nl();
				break;
			}
      else
        outbyte(ch);
		}
	}
}

/**********************************************************
*	dump to asm file global symbol list
**********************************************************/
void asdumpglbs(void)
{
	entry *p;	

	p=glbtab->start;
	while(p){								/* skip typedefs, idents and consts*/
		if((p->storage==TYPEDEF)||(p->ident==CONST)||(p->ident==IDENT)){
			p=p->next;
			continue;
		}

/* all but typedef!! */
		if((p->ident==FUNCTION)||(p->ident==PROTOTYPE)){	/* functions */
			ot(p->storage==EXTERN ? "extrn\t":"public\t");
			outstr(p->name);
			nl();
			p=p->next;
			continue;
		}

/* variables */
		ot(p->storage==EXTERN ? "extrn\t":"public\t");
		outstr(p->name);
		nl();
		
		underscore();
		outstr(p->name);
		switch(p->size){			/* only public?? */
			case 1: ot("db\t");
							outdec(p->misc.ivalue);
							break;
	
			case 2: ot("dw\t");
							outdec(p->misc.ivalue);
							break;
	
			default:{						/* ?????? */
							int i;
							
							ot("db\t");
							for(i=1;i<p->size;i++) outstr("0,");
							outstr("0");
							}
	
		}
		nl();
		
		p=p->next;
		continue;
	}
}
