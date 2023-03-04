/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   expr.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/*get rid of lval[] use struct entry!
 *	lval[0] - symbol table address, else 0 for constant
 *	lval[1] - type indirect object to fetch, else 0 for static object
 *	lval[2] - type pointer or array, else 0
 */

/*local functions */
int heir1a(entry *item);
int heir1b(entry *item);
int heir1c(entry *item);
int heir2(entry *item);
int heir3(entry *item);
int heir4(entry *item);
int heir5(entry *item);
int heir6(entry *item);
int heir7(entry *item);
int heir8(entry *item);
int heir9(entry *item);
int heir10(entry *item);
int heir11(entry *item);
void findmember(entry *ptr);

/*********************************************************
 * this function evaluates expression
 * detailed description needed.. (graph or tree)
 *********************************************************/
void expression(int comma)
{
    entry item;

    do {
        if(heir1(&item)) rvalue(&item);
        if(!comma) return;
    }while(match(","));
}


/*********************************************************
 * combined operators
 * =, -=, +=, *=, /=, %=, >>=, <<=, &=, ^=, |=
 **********************************************************/
int heir1(entry *item)
{
    int k;
    entry item2;
    char    fc;

    k=heir1a(item);
    if(match("=")) {
        if(k==0) {
            error("must be lvalue");
            return(0);
        }
        if(item->lval1) vmpush(item);
        if(heir1(&item2)) rvalue(&item2);
        store(item);
        return(0);
    }
    else {
        fc=ch();
        if(match("-=")||match("+=")||match("*=")||match("/=")||match("%=")||
        match(">>=")||match("<<=")||match("&=")||match("^=")||match("|=")) {
            if(k==0) {
                error("must be lvalue");
                return(0);
            }
            if(item->lval1) vmpush(item);
            rvalue(item);
            vmpush(item);
            if(heir1(&item2)) rvalue(&item2);
            switch(fc) {
                /*				case '-':	{
                                    if(dbltest(item,&item2))	gaslint();
                                    vmsub(item,&item2);
                                    result(item,&item2);
                                    break;
                                }
                                case '+':	{
                                    if(dbltest(item,&item2)) gaslint();
                                    vmadd(item,&item2);
                                    result(item,&item2);
                                    break;
                                }
                */
                case '+': vmadd(item,&item2);break;
                case '-': vmsub(item,&item2);break;

                case '*':   vmmult(item,&item2);break;
                case '/':   vmdiv(item,&item2); break;
                case '%':   vmmod(item,&item2); break;
                case '>':   vmsr(item,&item2); break;
                case '<':   vmsl(item,&item2); break;
                case '&':   vmand(item,&item2); break;
                case '^':   vmxor(item,&item2); break;
                case '|':   vmor(item,&item2);  break;
            }
            store(item);
            return(0);
        }else
        return(k);
    }
}


/**********************************************************
 * conditional expression ?:
 **********************************************************/
int heir1a(entry *item)
{
    int k,lab1,lab2;
    entry item2;

    k=heir1b(item);
    blanks();
    if(ch()!='?')   return(k);
    if(k)   rvalue(item);
    while(1)
    if(match("?")) {
        vmtestjump(lab1=getlabel(),FALSE);
        if(heir1b(&item2)) rvalue(&item2);
        vmjump(lab2=getlabel());
        printlabel(lab1);
        underscore();
        nl();
        blanks();
        if(!match(":")) {
            error("missing colon");
            return(0);
        }
        if(heir1b(&item2)) rvalue(&item2);
        printlabel(lab2);
        underscore();
        nl();
    }else
    return(0);
}


/**********************************************************
 * logical ||
 **********************************************************/
int heir1b(entry *item)
{
    int k,lab;
    entry item2;

    k=heir1c(item);
    blanks();
    if(!sstreq("||"))   return(k);
    if(k) rvalue(item);
    while(1)
    if(match("||")) {
        vmtestjump(lab=getlabel(),TRUE);
        if(heir1c(&item2)) rvalue(&item2);
        printlabel(lab);
        underscore();
        nl();
        vmbool();
    }else
    return(0);
}


/**********************************************************
 * logical &&
 **********************************************************/
int heir1c(entry *item)
{
    int k,lab;
    entry item2;

    k=heir2(item);
    blanks();
    if(!sstreq("&&"))   return(k);
    if(k) rvalue(item);
    while(1)
    if(match("&&")) {
        vmtestjump(lab=getlabel(),FALSE);
        if(heir2(&item2)) rvalue(&item2);
        printlabel(lab);
        underscore();
        nl();
        vmbool();
    }else
    return(0);
}


/**********************************************************
 * bitwise |
 **********************************************************/
int heir2(entry *item)
{
    int k;
    entry item2;

    k=heir3(item);
    blanks();
    if((ch()!='|') | (nch()=='|') | (nch()=='='))   return(k);
    if(k) rvalue(item);
    while(1) {
        if((ch()=='|') & (nch()!='|') & (nch()!='=')) {
            inchar();
            vmpush(item);
            if(heir3(&item2)) rvalue(&item2);
            vmor(item,&item2);
            blanks();
        }else
        return(0);
    }
}


/**********************************************************
 * bitwise ^
 **********************************************************/
int heir3(entry *item)
{
    int k;
    entry item2;

    k=heir4(item);
    blanks();
    if((ch()!='^') | (nch()=='=')) return(k);
    if(k)   rvalue(item);
    while(1) {
        if((ch()=='^') & (nch()!='=')) {
            inchar();
            vmpush(item);
            if(heir4(&item2)) rvalue(&item2);
            vmxor(item,&item2);
            blanks();
        }else
        return(0);
    }
}


/**********************************************************
 * bitwise &
 **********************************************************/
int heir4(entry *item)
{
    int k;
    entry item2;

    k=heir5(item);
    blanks();
    if((ch()!='&') | (nch()=='|') | (nch()=='='))   return(k);
    if(k) rvalue(item);
    while(1) {
        if((ch()=='&') & (nch()!='&') & (nch()!='=')) {
            inchar();
            vmpush(item);
            if(heir5(&item2)) rvalue(&item2);
            vmand(item,&item2);
            blanks();
        }else
        return(0);
    }
}


/**********************************************************
 * logical ==, !=
 **********************************************************/
int heir5(entry *item)
{
    int k;
    entry item2;

    k=heir6(item);
    blanks();
    if(!sstreq("==") & !sstreq("!="))   return(k);
    if(k)   rvalue(item);
    while(1) {
        if(match("==")) {
            vmpush(item);
            if(heir6(&item2)) rvalue(&item2);
            vmeq(item,&item2);
        }
        else if(match("!=")) {
            vmpush(item);
            if(heir6(&item2)) rvalue(&item2);
            vmne(item,&item2);
        }else
        return(0);
    }
}


/**********************************************************
 * logical <=, >=, >, <
 **********************************************************/
int heir6(entry *item)
{
    int k;
    entry item2;

    k=heir7(item);
    blanks();
    if(!sstreq("<") && !sstreq("<=") && !sstreq (">=") && !sstreq (">")) return(k);
    if(sstreq ("<<") || sstreq (">>")) return (k);

    if(k)   rvalue(item);
    while(1) {
        if(match("<=")) {
            vmpush(item);
            if(heir7(&item2)) rvalue(&item2);
            if(item->lval2 || item2.lval2) {
                vmule(item,&item2);
                continue;
            }
            vmle(item,&item2);
        }
        else if(match(">=")) {
            vmpush(item);
            if(heir7(&item2)) rvalue(&item2);
            if(item->lval2 || item2.lval2) {
                vmuge(item,&item2);
                continue;
            }
            vmge(item,&item2);
        }
        else if((sstreq("<")) && !sstreq("<<")) {
            inchar();
            vmpush(item);
            if(heir7(&item2)) rvalue(&item2);
            if(item->lval2 || item2.lval2) {
                vmult(item,&item2);
                continue;
            }
            vmlt(item,&item2);
        }
        else if((sstreq(">")) && !sstreq(">>")) {
            inchar();
            vmpush(item);
            if(heir7(&item2)) rvalue(&item2);
            if(item->lval2 || item2.lval2) {
                vmugt(item,&item2);
                continue;
            }
            vmgt(item,&item2);
        }else
        return(0);
        blanks();
    }
}


/**********************************************************
 * bitwise >>, <<
 **********************************************************/
int heir7(entry *item)
{
    int k;
    entry item2;

    k=heir8(item);
    blanks();
    if(!sstreq(">>") && !sstreq("<<") || sstreq(">>=") || sstreq("<<=")) return(k);
    if(k)   rvalue(item);
    while(1) {
        if(sstreq(">>") && ! sstreq(">>=")) {
            inchar();
            inchar();
            vmpush(item);
            if(heir8(&item2)) rvalue(&item2);
            vmsr(item,&item2);
        }
        else if(sstreq("<<") && !sstreq("<<=")) {
            inchar();
            inchar();
            vmpush(item);
            if(heir8(&item2)) rvalue(&item2);
            vmsl(item,&item2);
        }else
        return(0);
        blanks();
    }
}


/**********************************************************
 * unary +, -
 **********************************************************/
int heir8(entry *item)
{
    int k;
    entry item2;

    k=heir9(item);
    blanks();

    if((ch()!='+') && (ch()!='-') || nch()=='=') return(k);
    if(k) rvalue(item);
    while(1) {
        if(match("+")) {
            vmpush(item);
            if(heir9(&item2)) rvalue(&item2);
            /*			if(dbltest(item,&item2)) gaslint();*/
            vmadd(item,&item2);
            result(item,&item2);
        }
        else if(match("-")) {
            vmpush(item);
            if(heir9(&item2)) rvalue(&item2);
            /*			if(dbltest(item,&item2)) gaslint();*/
            vmsub(item,&item2);
            /*			if((item->lval2==INT) && (item2.lval2==INT))	gasrint();*/
            result(item,&item2);
        }else
        return(0);
    }
}


/**********************************************************
 * unary *, /, %
 **********************************************************/
int heir9(entry *item)
{
    int k;
    entry item2;

    k=heir10(item);
    blanks();
    if(((ch()!='*') && (ch()!='/') && (ch()!='%')) || (nch()=='=')) return(k);

    if(k)   rvalue(item);
    while(1) {
        if(match("*")) {
            vmpush(item);
            if(heir10(&item2)) rvalue(&item2);
            vmmult(item,&item2);
        }
        else if(match("/")) {
            vmpush(item);
            if(heir10(&item2)) rvalue(&item2);
            vmdiv(item,&item2);
        }
        else if(match("%")) {
            vmpush(item);
            if(heir10(&item2)) rvalue(&item2);
            vmmod(item,&item2);
        }else
        return(0);
    }
}


/*********************************************************
 * unary ++, --, -, ~, !, *(ptr), &(adr)
 **********************************************************/
int heir10(entry *item)
{
    int k;

    if(match("++")) {            /* ++x */
        if((k=heir10(item))==0) {
            error("Must be lvalue");
            return(0);
        }
        if(item->lval1) vmpush(item);
        rvalue(item);
        vminc(item);
        store(item);
        return(0);
    }
    else if(match("--")) {       /* --x */
        if((k=heir10(item))==0) {
            error("must be lvalue");
            return(0);
        }
        if(item->lval1) vmpush(item);
        rvalue(item);
        vmdec(item);
        store(item);
        return(0);
    }
    else if(match("-")) {        /* -x */
        k=heir10(item);
        if(k) rvalue(item);
        vmneg(item);
        return(0);
    }
    else if(match("~")) {        /* ~x */
        k=heir10(item);
        if(k)   rvalue(item);
        vmcom(item);
        return(0);
    }
    else if(match("!")) {        /* !x */
        k=heir10(item);
        if(k)   rvalue(item);
        vmlneg(item);
        return(0);
    }
                                 /* * pointer */
    else if(ch()=='*' && nch()!='=') {
        inchar();
        k=heir10(item);
        if(k)   rvalue(item);
        if(item->ident!=CONST)
            item->lval1=item->type;
        else
            item->lval1=INT;
        item->lval2=0;
        return(1);
    }
                                 /* & address */
    else if(ch()=='&' && nch()!='&' && nch()!='=') {
        inchar();
        k=heir10(item);
        if(k==0) {
            error("illegal address");
            return(0);
        }
        if(item->lval1) return(0);
        /* global and non-array */
        immed();
        prefix();
        outstr(item);
        underscore();
        nl();
        item->lval1=item->type;
        return(0);
    }
    else {
        k=heir11(item);
        if(match("++")) {        /* x++ */
            if(k==0) {
                error("must be lvalue");
                return(0);
            }
            if(item->lval1) vmpush(item);
            rvalue(item);
            vminc(item);
            store(item);
            vmdec(item);
            return(0);
        }
        else if(match("--")) {   /* x-- */
            if(k==0) {
                error("must be lvalue");
                return(0);
            }
            if(item->lval1) vmpush(item);
            rvalue(item);
            vmdec(item);
            store(item);
            vminc(item);
            return(0);
        }
        else
            return(k);
    }
}


/*********************************************************
 * !! here structs and unions should be processed
 *********************************************************/
int heir11(entry *item)
{
    int k;

    k=primary(item);
    blanks();

    if(ch()=='.') findmember(item);
    if((ch()=='-') && (nch()=='>')) warning("-> not implemented yet");

    if((ch()=='[')||(ch()=='(')) /* array and function */
    while(1) {
        if(match("[")) {
            if(item->ident==CONST) {
                error("can't subscript");
                junk();
                needbrack("]");
                return(0);
            }
            else if(item->ident==POINTER)
                rvalue(item);
            else if(item->ident!=ARRAY) {
                error("can't subscript");
                k=0;
            }
            vmpush(item);
            expression(TRUE);
            needbrack("]");
            if(item->type==INT) gaslint();
            vmadd(NULL,NULL);
            item->lval0=0;
            item->lval1=item->type;
            k=1;
        }else
        if(match("(")) {         /* function */
            if(item==NULL)
                callfunc(0);
            else if((item->ident!=FUNCTION)/*&&(item->ident!=PROTOTYPE)*/){
            rvalue(item);
            callfunc(0);
        }else
        callfunc(item);          /* function in expression */
        k=0;
        k=item->lval0=0;
    }
    else
        return(k);
}


/*	if(item->ident==CONST) return(k); */
if(item->lval0==0) return(k);
if(item->ident==FUNCTION) {      /* when this is called ??? */
    immed();
    prefix();
    outstr(item->name);
    underscore();
    nl();
    return(0);
}


return(k);
}


/**********************************************************
 * find a member in a struct/union
 * ONLY simple non recursive structs are processed
 * REWRITE
 **********************************************************/
void findmember(entry *ptr)
{
    entry *sptr;
    char name[NAMESIZE];

    sptr=ptr->further->start;
    if(match(".")) {
        if(!symname(name)) error("Illegal symbol name");

        while(sptr) {
            if(strcmp(name,sptr->name)==0) break;
            sptr=sptr->next;
        }
        if(sptr==0) error("not a struct or union member");

        /* we must verify member types
        if((ptr->ident!=VARIABLE)||(sptr->ident!=VARIABLE))
            error("struct/union type mismatch");
        */

                                 /* offset from base name VERIFY*/
        ptr->offset+=sptr->offset;
                                 /* new storage */
        ptr->storage=sptr->storage;
        ptr->type=sptr->type;    /* new type */
        ptr->ident=sptr->ident;  /* new ident */
        ptr->size=sptr->size;    /* new size */
    }
}
