/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   main.c
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
#include <stdio.h>
#include <limits.h>
#include "defs.h"
#include "data.h"
#include "externs.h"

#ifndef MAXPATH
#define MAXPATH PATH_MAX
#endif

/* local functions */
void main(int argc, char *argv[]);
char *foname(char *name);
void version(void);
void usage(void);
void parse(void);

/* temporary functions */
void debugmacros(void);
void dodebug(list *root,int f);
void debuglist(list *root,char *name);
void debugone(char *str,entry *p);

int errs=0;

/**********************************************************
* main compiler function
***********************************************************/
void main(int argc, char *argv[])
{
	char	*p;
	char *bp=NULL;

	argv++;
	ctext=FALSE;
	errs=0;
	aflag=1;
  TRACE=0;										/* imbed trace info into asm file */

	while((p=*argv++))
		if(*p=='-')	while(*++p)
			switch(*p){
				case 't':								/* append source to asm output*/
					ctext=1;
					break;
					
					case 'a':								/* indicate # of args in functions */
						aflag=0;
						break;
					
					case 'd':
						bp=++p;
						if(!*p) usage();
						while(*p && *p != '=') p++;
						if(*p=='=') *p='\t';
						while(*p) p++;
						p--;
						defmac(bp);
						break;

					case 'o':								/* specify output name */
						bp=++p;									
						while(*p) p++;
						p--;
						break;
						
					case 'r':								/* debug compiler */
						TRACE=1;
        		break;
				
					default:
						usage();
			}
			else
				break;

	if(!p) usage();

	while(p){
/* better allow one source file only.. */
		errfile=0;
		if((input=fopen(p,"r"))==NULL) fatal("Open failure",1);
		if((output=fopen(*bp? bp:foname(p),"w"))==NULL) fatal("Open failure",2);


		glbtab=(list *)calloc(1,sizeof(list));				/* init global symbol table */
		loctab=(list *)calloc(1,sizeof(list));				/* init local symbol table */

		startws=wsptr=(wsentry *)calloc(NUMWS,sizeof(wsentry));
		endws=startws+NUMWS;

		inclsp=swstp=litptr=stkp=ncmp=0;

		input2=NULL;
		cmode=TRUE;

/* this allows the addition of other typedefs */
		addsymbol(glbtab,"char",TYPEDEF,CHAR,VARIABLE,CHARSIZE);
		addsymbol(glbtab,"int",TYPEDEF,INT,VARIABLE,INTSIZE);
		addsymbol(glbtab,"float",TYPEDEF,FLOAT,VARIABLE,FLOATSIZE);

/* what about command line options like -Isize=.., -Fsize=.. ?*/
/* dynamic change of variable size?? */
/* why not use optional config file eg: mediumc.rc */

		litlab=getlabel();

		asheader();
		astext();
		parse();
		fclose(input);

		asdumplits();
    asdata();
		asdumpglbs();
		errorsummary();
		astrailer();

		if(TRACE){
			debuglist(glbtab,"global");
			debugmacros();
		}

		fclose(output);
		fputs("\n",stderr);
		errs=errs || errfile;

		p=*argv++;
	}

	rmtable(glbtab);					/* remove glbtab */
	rmmac();									/* remove macro list */		
	exit(errs!=0);						/* verify exit codes */
}

/**********************************************************
*	make file output name
**********************************************************/
char *foname(char *name)
{
	char fname[MAXPATH];
	char *x;
	strncpy(fname, name, MAXPATH);
	x = strrchr(fname, '.');
	strcpy(x, ".s");				/* append .s */
	return(fname);
}

/**********************************************************
* return compiler version ID
**********************************************************/
void version(void)
{
	fputs(VERSION,stderr);
}

/**********************************************************
* print usage msg
**********************************************************/
void usage(void)
{
	fputs("usage: mcc [-art] [-d<sym>[=<value>]] [-o<OutputFile>] <InputFile>\n\n", stderr);
	fputs("options:\n",stderr);
	fputs("-a\t- indicate #of args passed into functions\n",stderr);
	fputs("-r\t- trace compiler calls (for internal debuging only)\n",stderr);
	fputs("-t\t- append source lines into OutputFile\n",stderr);
	fputs("-d\t- define preprocessor symbol\n",stderr);
	fputs("-o\t- set OutputFile name\n", stderr);
	exit(1);
}

/**********************************************************
*	process all input text
*	process keywords:
* extern, static, public, #define, #undef, #include, #asm,
**********************************************************/
void parse(void)
{
	while(!feof(input)){
		if(match("extern"))
			doglobal(EXTERN);
		else if(match("static"))
			doglobal(STATIC);
		else if(match("public"))
			doglobal(PUBLIC);
		else if(match("#asm"))
			doasm();
		else if(match("#include"))
			doinclude();
		else if(match("#define"))
			dodefine();
		else if(match("#undef"))
			doundef();
		else
			doglobal(PUBLIC);					/* assume public */
		blanks();
	}
}


/* various ad hoc functions for debugging purposes */

/********************************************************/
void debugmacros(void)
{
/*
OLD
char *k;
k=macstart;
while(k<macptr) printf("%c",*k++);
*/
macentry *ptr;

printf("macro list:\n");
ptr=macstart;
while(ptr){
	printf("lit: %10s def: %s\n",ptr->lit,ptr->def);
	ptr=ptr->next;
}
}

/*=======================================================*/
char id[7][8]={"CONST","VAR","ARRAY","POINTER",
									 "FUNCT","PROTO","IDENT"};
char tp[8][8]={"none","CHAR","INT","FLOAT","VOID",
									"ENUM","STRUCT","UNION"};
char st[7][8]={"PUBLIC","AUTO","EXTERN","STATIC","LSTATIC",
											"DEFAUTO","TYPEDEF"};

/********************************************************/
void dodebug(list *root,int f)
{
	char name[NAMESIZE+1];
	entry *p;
	
	p=root->start;
	while(p){
		sprintf(name,"%s%s",(f==1?"+":" "),p->name);
		printf("%9s%8s%8s%8s%7d%8d%8d%8d%8d%8d",
			name,st[p->storage],tp[p->type],id[p->ident],
			p->size,p,p->next,p->prev,p->further->start,p->stkoffs /*offset*/);
if(p->endstmt) printf("--------------\n");						
		if( (((p->type==UNION)||(p->type==STRUCT))&&(p->ident==IDENT)) ||
			(p->ident==FUNCTION)||(p->ident==PROTOTYPE))
			dodebug(p->further,1);
		p=p->next;
	}
}

/********************************************************/
void debuglist(list *root,char *name)
{
	printf("debug: %s list, at: %d, start: %d\n",name,root,root->start);
	printf("     name   class    type   ident   size    adr    next    prev   furth      val");
	dodebug(root,0);
}

void debugone(char *str,entry *p)
{
	printf("%s\n",str);
	printf("     name   class    type   ident   size    offset   stkoffs  ival\n");
	printf("%9s%8s%8s%8s%8d%8d%8d%8d\n",
			p->name,st[p->storage],tp[p->type],id[p->ident],
			p->size,p->offset,p->stkoffs,p->misc.ivalue);
}
