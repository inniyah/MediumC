/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   preproc.c
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
FILE *fixiname(void);
char *findmac(char *sname);
void doifdef(int ifdef);
int ifline(void);
int cpp(void);
char keepch(char c);

int iflevel=0;
int skiplevel=0;

/*********************************************************
*	open an include file
*********************************************************/
void doinclude()
{
	char	*p;
	FILE	*inp2;

	blanks();
	if((inp2=fixiname())!=NULL)
		if(inclsp<INCLSIZ){
			inclstk[inclsp++]=input2;
			input2=inp2;
		}else{
			fclose(inp2);
			error("too many nested includes");
		}
	else{
		fatal("Could not open include file",3);
	}
	kill();
}

/*********************************************************
*	remove "brackets" around include file name
* return opened file or NULL
*
* modify to enable access to INCLUDE directory
*********************************************************/
FILE *fixiname(void)
{
	char	c1, c2, *p, *ibp;
	char buf[20];
	FILE *fp;

	ibp=&buf[0];
	if((c1=gch())!='"' && c1!='<') return(NULL);

	for(p=line+lptr; *p;) *ibp++=*p++;
	c2=*(--p);
	if(c1=='"' ? (c2!='"'):(c2!='>')){
		error("incorrect delimiter");
		return(NULL);
	}
	*(--ibp)=0;
	fp=NULL;
	if((c1=='<') || !(fp=fopen(buf,"r"))){
		fp=fopen(buf,"r");
	}
	return(fp);
}

/*********************************************************
*	"asm" pseudo-statement
*	enters mode where assembly language statements are passed
*	intact through parser
*********************************************************/
void doasm(void)
{
	cmode=FALSE;
	while(1){
		finline();
		if(match("#endasm")) break;
		if(feof(input))	break;
		outstr(line);
		nl();
	}
	kill();
	cmode=TRUE;
}

/*********************************************************
* define macro whose definition is pointed by mac
*********************************************************/
void defmac(char *mac)
{
	kill();
	strcpy(line,mac);
	dodefine();
}

/*********************************************************
* define macro whose defintion is pointed by lptr
*********************************************************/
void dodefine(void)
{
	macentry *ptr;
	char sname[NAMESIZE];

	if(!symname(sname)){									/* check for valid name */
		error("Illegal symbol name");
		kill();
		return;
	}
	if(findmac(sname)){										/* check for duplicate define */
		error("Duplicate define");
		delmac(sname);
	}
	ptr=(macentry *)calloc(1,sizeof(macentry));				/* make new entry */
	ptr->lit=strdup(sname);								/* save new literal */
	while(ch()==' ' || ch()==TAB)	gch();	/* skip spaces and tabs */
	ptr->def=strdup(&line[lptr]);					/* save macro definition */
	while(gch());													/* advance lptr, real heck! */

	ptr->next=macstart;										/* put entry onto the list */
	macstart=ptr;
}

/*********************************************************
* delete macro pointed by lptr
*********************************************************/
void doundef(void)
{
	char sname[NAMESIZE];

	if(!symname(sname)){
		error("Illegal symbol name");
		kill();
		return;
	}

	delmac(sname);
	kill();
}

/*********************************************************
* return pointer to macro expansion or null
*********************************************************/
char *findmac(char *sname)
{
	macentry *ptr;

	ptr=macstart;
	while(ptr){
		if(strcmp(sname,ptr->lit)==0) return(ptr->def);
		ptr=ptr->next;
	}
	return(NULL);
}

/*********************************************************
* undefine given macro
*********************************************************/
void delmac(char *mac)
{
	macentry *ptr,*last;

	ptr=macstart;
	while(ptr){											/* find literal */
		if(strcmp(mac,ptr->lit)==0){
			last->next=ptr->next;				/* remove from list */
			free(ptr->lit);							/* free the entry */
			free(ptr->def);
			free(ptr);
			return;											/* done */
		}
		last=ptr;											/* next entry */
		ptr=ptr->next;
	}
}

/*********************************************************
* remove the entire macro list
*********************************************************/
void rmmac(void)
{
	macentry *ptr;
	
	ptr=macstart;
	while(ptr){
		free(ptr->lit);
		free(ptr->def);
		ptr=ptr->next;
	}
}


/*********************************************************/
void preprocess(void)
{
	if(ifline()) return;
	while(cpp());
}

/*********************************************************/
void doifdef(int ifdef)
{
	char sname[NAMESIZE];
	int k;

	blanks();
	++iflevel;
	if(skiplevel) return;
	k=symname(sname) && findmac(sname); /* tu moze byc klopot */
	if(k!=ifdef) skiplevel=iflevel;
}

/*********************************************************
* process #ifdef,#ifndef,#else,#endif
*********************************************************/
int ifline(void)
{
	while(1){
		finline();
		if(feof(input)) return(1);
		
		if(match("#ifdef")){
			doifdef(TRUE);
			continue;
		}else if(match("#ifndef")){
			doifdef(FALSE);
			continue;
		}else if(match("#else")){
			if(iflevel){
				if(skiplevel==iflevel) skiplevel=0;
				else if(skiplevel==0) skiplevel=iflevel;
			}else error("No matching #if");
			continue;
		}else if(match("#endif")){
			if(iflevel){
				if(skiplevel==iflevel) skiplevel=0;
				--iflevel;
			}else error("No matching #if");
			continue;
		}
		if(!skiplevel) return(0);
	}
}

char mline[LINESIZE];			/* temp preprocessor line */
int mptr=0;

/*********************************************************
* Preprocessor
*********************************************************/
int cpp(void)
{
	int	k;
	char c, *ptr, sname[NAMESIZE];
	int	tog;
	int	cpped;		/* non-zero if something expanded */

	cpped=0;
/* don't expand lines with preprocessor commands in them */
	if((cmode==FALSE) || line[0]=='#') return(0);

	mptr=0;
	lptr=0;
	while(ch()){
		if((ch()==' ')||(ch()==TAB)){
			keepch(' ');
			while((ch()==' ')||(ch()==TAB)) gch();
		}else if(ch()=='"'){
			keepch(ch());
			gch();
			while(ch()!='"'){
				if(ch()==0){
					error("missing quote");
					break;
				}
				if(ch()=='\\') keepch(gch());
				keepch(gch());
			}
			gch();
			keepch('"');
		}else if(ch()=='\''){
			keepch('\'');
			gch();
			while(ch()!='\''){
				if(ch()==0){
					error("missing apostrophe");
					break;
				}
				if(ch()=='\\') keepch(gch());
				keepch(gch());
			}
			gch();
			keepch('\'');

		}else if((ch()== '/') && (nch()=='*')){	/* C comment */
			inbyte();
			inbyte();
			while((((c=ch())=='*') && (nch()=='/'))==0){
				if(ch()==0)
  				finline();
				else
					inbyte();
				if(feof(input))	break;
			}
			inbyte();
			inbyte();
	  }else
			if((ch()=='/') && (nch()=='/')){					/* C++ comment */	  
				while(ch()!=0){
 					inbyte();
 					if(feof(input)) break;
				}
			}else
	  	  if(alnum(ch())){											/* get sname */
				k=0;
				while(alnum(ch())){
					if(k<NAMEMAX)	sname[k++]=ch();
					gch();
				}
			sname[k]=0;

			if((ptr=findmac(sname))){						/* is it macro ? */
				cpped=1;													/* signal that found macro */
				while(c=*ptr++) keepch(c);				/* copy definition to mline */
			}else{
				k=0;
				while((c=sname[k++]))	keepch(c);	/* or sname to mline */
			}
		}else
			keepch(gch());
	}

	keepch(0);
	if(mptr>=LINESIZE)	error("line too long");

	lptr=0;
	mptr=0;

	while((line[lptr++]=mline[mptr++])); 		/* move mline to line */

	lptr=0;
	return(cpped);
}

/*********************************************************
* move char to temp preprocessor line
*********************************************************/
char keepch(char c)
{
	mline[mptr]=c;
	if(mptr<LINESIZE) mptr++;
	return(c);
}
