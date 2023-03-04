/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   vm.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/* work out a new speed/size optimisation*/
int M_lib=FALSE;                 /* see if we have to load MULT library */
int S_lib=FALSE;                 /* see if we need shift library */
int G_lib=FALSE;                 /* see if we need comp library */

/* Virtual Machine implementation */

/************************************************************
 * all functions should be aware about the size of operands
 * for now only one byte size is implemented
 **************************************************************/

/* Register and memory specific functions */
/********************************************************
 * fetch a static memory cell into the primary register
 *********************************************************/
void vmgetmem(entry *item)
{

    if((item->ident!=POINTER) && (item->type==CHAR)) {
        ot("mov\t_primary,");
        outstr(item->name);
        underscore();
        if(item->offset) {
            outstr("+");
            outdec(item->offset);
        }
        nl();
    }
    else {
        ot("mov\t_primary,");
        outstr(item->name);
        if(item->offset) {       /* for global structs/unions */
            outstr("+");
            outdec(item->offset);
        }
        nl();
    }
}


/**********************************************************
 * store the primary register into the specified static
 * memory cell
VERIFY..
***********************************************************/
void vmputmem(entry *item)
{
    if((item->ident!=POINTER) && (item->type==CHAR)) {
        ot("mov");
        ot(item->name);
        underscore();
        if(item->offset) {
            outstr("+");
            outdec(item->offset);
        }
        outstr(",_primary");
    }
    else {
        ot("mov");
        ot(item->name);
        underscore();
        if(item->offset) {
            outstr("+");
            outdec(item->offset);
        }
        outstr(",_primary");
    }
    nl();
}


/**************************************************************
 * fetch the address of the specified local symbol
 * into the primary register
 ***************************************************************/
void vmgetloc(entry *item)
{
    if(TRACE) fprintf(output,";vmgetloc(%s)\n",item->name);

    immed();
    if(item->storage==LSTATIC) {
        printlabel(item->offset);/* item->name */
        nl();
    }
    else {
        outdec(item->stkoffs-stkp);
        nl();
        ol("add\t_primary,_stackptr");
    }
}


/*************************************************************
 * store the specified object type in the primary register
 * at the address on the top of the stack
 **************************************************************/
void vmputstk(entry *item)
{
    vmpop();                     /* vmpop(item) */
    if(item->type==CHAR) {
        vmcall("_putstk");
    }
    else {
        vmcall("_putstk");
    }
}


/**********************************************************
 * fetch the specified object type indirect through the primary
 * register into the primary register
 ************************************************************/
void vmindirect(entry *item)
{
    if(item->type==CHAR) {
        vmcall("_indr");
    }
    else {
        vmcall("_indr");
    }
}


/***********************************************************
 * swap the primary and secondary registers
 ************************************************************/
void vmswap(void)
{
    if(TRACE) fprintf(output,";vmswap\n");

    vmcall("_swap");
}


/***********************************************************
 * print partial instruction to get an immediate value into
 * the primary register
 ************************************************************/
void immed(void)
{
    ot("mov\t_primary,#");
}


/* Arithmetic and logical functions */
/* in the following functions, shouldn't vmpop be performed
 * by higher level functions ? */

/***********************************************************
 * add the primary and secondary registers
 * if lval2 is int pointer and lval is not, scale lval
P=P+S
************************************************************/
void vmadd(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmadd(%s,%s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    ol("add\t_primary,_secondary");
}


/***********************************************************
 * subtract the primary register from the secondary
 * P=S-P;
 ************************************************************/
void vmsub(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmsub(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    S_lib=TRUE;
    vmcall("_sub");
    /*****
        ol("mov\t_temp,_secondary");
        ol("sub\t_temp,_primary");
        ol("mov\t_primary,_temp");
    ******/
}


/***********************************************************
 * multiply the primary and secondary registers
 * (result in primary)
P=P*S
************************************************************/
void vmmult(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmmult(%s, %s)\n",itemp->name, items->name);

    M_lib=TRUE;
    vmpop();                     /* ??? */
    vmcall("_mul");
}


/***********************************************************
 * divide the secondary register by the primary
 * (quotient in primary, remainder in secondary)
 ************************************************************/
void vmdiv(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmdiv(%s, %s)\n",itemp->name,items->name);

    M_lib=TRUE;
    vmpop();                     /* ??? */
    vmcall("_div");
}


/***********************************************************
 * compute the remainder (mod) of the secondary register
 * divided by the primary register
 * (remainder in primary, quotient in secondary)
 ************************************************************/
void vmmod(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmmod(%s, %s)\n",itemp->name,items->name);

    M_lib=TRUE;
    vmdiv(itemp,items);
    vmswap();
}


/***********************************************************
 * inclusive 'or' the primary and secondary registers
 ***********************************************************/
void vmor(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmor(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    ol("or\t_primary,_secondary");
}


/***********************************************************
 * exclusive 'or' the primary and secondary registers
 ***********************************************************/
void vmxor(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmxor(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    ol("mov\tW,_secondary");
    ol("xor\t_primary,W");
}


/***********************************************************
 * 'and' the primary and secondary registers
 ***********************************************************/
void vmand(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmand(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    ol("and\t_primary,_secondary");
}


/***********************************************************
 * arithmetic shift right the secondary register the number of
 * times in the primary register
 * (results in primary register)
 ***********************************************************/
void vmsr(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmsr(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    S_lib=TRUE;
    vmcall ("_asr");
    /**********
      ol("clc");
      ol("mov\tW,>>_secondary");
      ol("mov\t_secondary,W");
      ol("djnz\t_primary,$-3");
      ol("mov\t_primary,_secondary");
    **********/
}


/***********************************************************
 * arithmetic shift left the secondary register the number of
 * times in the primary register
 * (results in primary register)
 ************************************************************/
void vmsl(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmsl(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    S_lib=TRUE;
    vmcall("_asl");
    /*******
      ol("clc");
      ol("mov\tW,<<_secondary");
      ol("mov\t_secondary,W");
      ol("djnz\t_primary,$-3");
      ol("mov\t_primary,_secondary");
    *******/
}


/***********************************************************
 * two's complement of primary register
 ************************************************************/
void vmneg(entry *item)
{
    if(TRACE) fprintf(output,";vmneg(%s)\n",item->name);

    ol("neg\t_primary");
}


/***********************************************************
 * logical complement of primary register
 ***********************************************************/
void vmlneg(entry *item)
{
    if(TRACE) fprintf(output,";vmlneg(%s)\n",item->name);

    /* vmcall ("_logneg");  */
    ol("mov\tW,#1");
    ol("xor\t_primary,W");
}


/***********************************************************
 * one's complement of primary register
 ************************************************************/
void vmcom(entry *item)
{
    if(TRACE) fprintf(output,";vmcom(%s)\n",item->name);

    ol("not\t_primary");
}


/***********************************************************
 * convert primary value into logical value (0 if 0, 1 otherwise)
 ************************************************************/
void vmbool(void)
{
    if(TRACE) fprintf(output,";vmbool()\n");

    G_lib=TRUE;
    vmcall("_bool");
    /********
        ol("mov\tW,_primary");
        ol("jnz\t$+3");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *********/
}


/***********************************************************
 * increment the primary register by 1 if char, INTSIZE if int
 ************************************************************/
void vminc(entry *item)
{
    if(TRACE) fprintf(output,";vminc(%s)\n",item->name);

    ol("inc _primary");
    if(item->type==INT) {        /* wrong !, what about carry */
        ol("inc\t_primary");
        ol("inc\t_primary");
    }
}


/***********************************************************
 * decrement the primary register by one if char, INTSIZE if int
 ************************************************************/
void vmdec(entry *item)
{
    if(TRACE) fprintf(output,";vmdec(%s)\n",item->name);

    ol("dec _primary");
    if(item->type==INT) {        /* wrong!, what about borrow */
        ol("dec\t_primary");
        ol("dec\t_primary");
    }
}


/***********************************************************
 * following are the conditional operators.
 * they compare the secondary register against the primary register
 * and put a literl 1 in the primary if the condition is true,
 * otherwise they clear the primary register
 ***********************************************************/

/***********************************************************
 * equal
 ************************************************************/
void vmeq(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmeq(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_geq");
    /******
        ol("csne\t_primary,_secondary");
        ol("jmp\t$+3");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *******/
}


/***********************************************************
 * not equal
 ************************************************************/
void vmne(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmne(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ???? */
    G_lib=TRUE;
    vmcall("_gne");
    /*******
        ol("cse\t_primary,_secondary");
        ol("jmp\t$+4");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    ********/
}


/***********************************************************
 * less than
 ************************************************************/
void vmlt(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmlt(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_glt");
    /********
        ol("cjb\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    ********/
}


/***********************************************************
 * less than or equal
 ************************************************************/
void vmle(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmle(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_gle");
    /********
        ol("cjbe\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *********/
}


/***********************************************************
 * greater than
 ************************************************************/
void vmgt(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmgt(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_ggt");
    /********
        ol("cja\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    ********/
}


/***********************************************************
 * greater than or equal
 ************************************************************/
void vmge(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmge(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_gge");
    /******
        ol("cjae\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *******/
}


/***********************************************************
 * less than
 ************************************************************/
void vmult(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmult(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_gult");
    /*******
        ol("cjb\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *******/
}


/***********************************************************
 * less than or equal
 ************************************************************/
void vmule(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmule(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_gule");
    /*******
        ol("cjbe\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *******/
}


/***********************************************************
 * greater than
 ************************************************************/
void vmugt(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmugt(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_gugt");
    /********
        ol("cja\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    *********/
}


/***********************************************************
 * greater than or equal
 ************************************************************/
void vmuge(entry *itemp, entry *items)
{
    if(TRACE) fprintf(output,";vmuge(%s, %s)\n",itemp->name,items->name);

    vmpop();                     /* ??? */
    G_lib=TRUE;
    vmcall("_guge");
    /**********
        ol("cjae\t_secondary,_primary,$+7");
        ol("mov\t_primary,#0");
        ol("skip");
        ol("mov\t_primary,#1");
    **********/
}


/* Call and jump functions */
/***********************************************************
 * call the specified subroutine name
 ***********************************************************/
void vmcall(char *sname)
{
    if(TRACE) fprintf(output,";vmcall(%s)\n",sname);

    ot("call\t");
    outstr(sname);
    underscore();
    nl();
}


/***********************************************************
 * return from subroutine
 ***********************************************************/
void vmret(void)
{
    if(TRACE) fprintf(output,";vmret()\n");

    ol("ret");
}


/***********************************************************
 * perform subroutine call to value on top of stack
 ***********************************************************/
void vmcallstk(void)
{
    if(TRACE) fprintf(output,";vmcallstk()\n");

    return;
    /******
        immed();
        outstr("$+5");
        nl();
        vmswapstk();
        ol("pchl");
        stkp=stkp+INTSIZE;
    ******/
}


/***********************************************************
 * jump to specified internal label number
 ***********************************************************/
void vmjump(int label)
{
    if(TRACE) fprintf(output,";vmjump(%d)\n",label);

    ot("jmp\t");
    printlabel(label);
    underscore();
    nl();
}


/***********************************************************
 * test the primary register and jump if false to label
zly opis!!!
************************************************************/
void vmtestjump(int label, int ft)
{
    if(TRACE) fprintf(output,";vmtestjump(%d,%d)\n",label,ft);

    ol("mov\tW,_primary");
    if(ft)
        ot("jnz\t");
    else
        ot("jz\t");
    printlabel(label);
    underscore();
    nl();
}


/* Stack specific functions */
/***********************************************************
 * push the primary register onto the stack
 ************************************************************/
void vmpush(entry *item)
{
    if(TRACE) fprintf(output,";vmpush(%s)\n",item->name);
    /*****
        ol("dec\t_stackptr");
        ol("mov\t_fr4,_stackptr");
        ol("mov\t_fr0,_primary");
    ******/
    vmcall("_push");
    stkp=stkp-INTSIZE;           /* item->size */
}


/***********************************************************
 * pop the top of the stack into the secondary register
shoudn't we have vmpopprim() and vmpopsec()?
***********************************************************/
void vmpop(void)
{
    if(TRACE) fprintf(output,";vmpop\n");
    /*****
        ol("mov\t_fr4,_stackptr");
        ol("mov\t_secondary,_fr0");
        ol("inc\t_stackptr");
    *****/
    vmcall("_pop");
    stkp=stkp+INTSIZE;           /* item->size */
}


/***********************************************************
 * swap the primary register and the top of the stack
 ***********************************************************/
void vmswapstk(void)
{
    if(TRACE) fprintf(output,";vmswapstk()\n");
    vmcall("_swaps");
    /*****
        ol("mov\t_fr4,_stackptr");
        ol("mov\t_temp,_fr0");
        ol("mov\t_fr0,_primary");
        ol("mov\t_primary,_temp");
    *****/
}


/***********************************************************
 * modify the stack pointer to the new value indicated
 ************************************************************/
int vmmodstk(int newstkp)
{
    int k;
    char buffer[50];

    if(TRACE) fprintf(output,";vmmodstk(%d)\n",newstkp);

    k=newstkp-stkp;
    if(k==0) return(newstkp);

    /* forget about stack alignment */
    if(k>0)
        sprintf(buffer,"add\t_stackptr,#%d",k);
    else
        sprintf(buffer,"sub\t_stackptr,#%d",abs(k));

    ol(buffer);
    return(newstkp);
}
