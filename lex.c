/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   lex.c
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#include <stdio.h>
#include <ctype.h>

#include "defs.h"
#include "data.h"
#include "externs.h"

/**********************************************************
 *	semicolon enforcer
 *	called whenever syntax requires a semicolon
 **********************************************************/
void issemicolon(void)
{
    if(!match(";")) error("missing semicolon");
}


/**********************************************************
 * skip over a junk text
 **********************************************************/
void junk(void)
{
    if(alnum(inchar()))
        while(alnum(ch())) gch();
    else
    while(alnum(ch())) {
        if(ch()==0) break;
        gch();
    }
    blanks();
}


/**********************************************************
 * return 1 if ; or NULL
 **********************************************************/
int endst(void)
{
    blanks();
    return((streq(line+lptr,";") || (ch()==0)));
}


/**********************************************************
 * test for a bracket: (,),[,]
 **********************************************************/
void needbrack(char *str)
{
    if(!match(str)) {
        error("missing bracket");
        comment();
        outstr(str);
        nl();
    }
}


/**********************************************************
 *	test if given character is ASCII or underscore
 **********************************************************/
int alpha(char c)
{
    return(isalpha(c)||(c=='_'));
}


/**********************************************************
 *	test if given character is alphanumeric or underscore
 ***********************************************************/
int alnum(char c)
{
    return((alpha(c))||(isdigit(c)));
}


/**********************************************************/
int sstreq(char *str1)
{
    return(streq(line+lptr,str1));
}


/**********************************************************
 * return length of str1 if equal to str2 or NULL
 ***********************************************************/
int streq(char *str1, char *str2)
{
    int k;

    k=0;
    while(str2[k]) {
        if((str1[k]!=str2[k])) return(0);
        k++;
    }
    return(k);
}


/*********************************************************
 *********************************************************/
int astreq(char *str1, char *str2, int len)
{
    int k;

    k=0;
    while(k<len) {
        if((str1[k]!=str2[k])) break;
        if(str1[k]==0) break;
        if(str2[k]==0) break;
        k++;
    }
    if(alnum(str1[k])) return(0);
    if(alnum(str2[k])) return(0);
    return(k);
}


/**********************************************************
 * if string found point to next element
 **********************************************************/
int match(char *lit)
{
    int k;

    blanks();
    if((k=(int)streq(line+lptr,lit))) {
        lptr=lptr+k;
        return(1);
    }
    return (0);
}


/*********************************************************
 * skip over blanks
 *********************************************************/
void blanks(void)
{
    while(1) {
        while(ch()==0) {
            preprocess();
            if(feof(input)) break;
        }
        if(ch()==' ')
            gch();
        else if(ch()==9)
            gch();
        else
            return;
    }
}
