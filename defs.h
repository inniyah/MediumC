/**********************************************************
 * MediumC PIC C Crosscompiler.  version: beta
 *
 * file:   defs.h
 * date:   January 1999
 *
 * author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
 **********************************************************/

#define VERSION "\tMediumC PIC C cross compiler, version: beta\n"

#define FALSE   0
#define TRUE    1

#define ALL     0
#define PART    1

/* miscellaneous */
#define EOS 0
#define EOL 10
#define BS  8
#define CR  13
#define FF  12
#define TAB 9
#define ESC 27

/* system-wide name size (for symbols) */
#define NAMESIZE    9
#define NAMEMAX     8

/* "switch" label stack size */
#define SWSTSZ  100

/* literal pool */
#define LITABSZ 2000
#define LITMAX  LITABSZ-1

/* input line */
#define LINESIZE    150
#define LINEMAX     (LINESIZE-1)

/* "include" stack */
#define INCLSIZ 3

/* fixup for case code generation */
#define CASE        0
#define DEFAULT 1

/* size of primary data elements */
#define CHARSIZE        1
#define INTSIZE         1
#define ENUMSIZE        1
#define FLOATSIZE       1
#define POINTERSIZE 1

#define LIBDIR  ".\\include"
#define MAXERR  10

/**********************************************************************/
/* possible entries for "ident" */
typedef enum {CONST,VARIABLE,ARRAY,POINTER,FUNCTION,PROTOTYPE,IDENT}
T_ident;

/* possible entries for "type" */
typedef enum {none,CHAR,INT,FLOAT,VOID,ENUM,STRUCT,UNION}
T_type;

/* possible entries for storage  KLASA*/
typedef enum {PUBLIC,AUTO,EXTERN,STATIC,LSTATIC,DEFAUTO,TYPEDEF}
T_storage;

/* statement types (tokens) */
typedef enum
{
    STIF,STWHILE,STRETURN,STBREAK,STCONT,STASM,
    STEXP,STDO,STFOR,STSWITCH
} T_tokens;

#define NUMWS           100

typedef union                    /* initialised values */
{
    char cvalue;
    int ivalue;                  /* enums */
    float fvalue;
}T_misc;

typedef struct entry
{
    char name[NAMEMAX];
    T_storage storage;
    T_type type;
    T_ident ident;
    unsigned int size;           /* size (bytes) of argument */
    int offset;                  /* offset for struct/union */
    int stkoffs;                 /* offset from stack ptr */
    T_misc misc;
    int endstmt;                 /* marks top of recursive stetement */
    int lval0;                   /* get rid of this! */
    int lval1;
    int lval2;
    struct list *further;        /* for functions, structs and unions */
    struct entry *prev;          /* this is a linked list */
    struct entry *next;
}entry;

typedef struct list
{
    entry *start;                /* start of the list */
    entry *end;                  /* end of the list */
    int entries;                 /* # of entries */
}list;

/* possible entries for "wstyp" */
typedef enum {WSWHILE,WSFOR,WSDO,WSSWITCH}
T_wstyp;

/* some fields in wsentry have multiple names */
#define wscasep wstest
#define wsincr wsdef
#define wsbody wstab

typedef struct
{
    entry *wssym;
    unsigned int wssp;
    T_wstyp wstyp;
    unsigned int wstest;
    unsigned int wsdef;
    unsigned int wstab;
    unsigned int wsexit;
}wsentry;

/* macro expansion linked list */
typedef struct macentry
{
    char *lit;                   /* new literal */
    char *def;                   /* its expansion */
    struct macentry *next;       /* this is a linked list */
}macentry;
