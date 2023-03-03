/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   externs.h
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
/* listmng.c */
extern entry *findglb(list *root,char *sname);
extern entry *findloc(list *root,char *sname,int part);
extern entry *findtype(char *sname);
extern entry *find(char *sname);
extern void rmtable(list *root);
extern entry *markend(void);
extern void trimtable(entry *item);
extern entry *addsymbol(list *root,char *sname,T_storage stor,T_type typ,T_ident id, unsigned int size);

/* function.c */
extern void newfunc(char *sname, T_type typ);
extern void callfunc(char *fname);

/* error.c */
extern void error(char *err);
extern void warning(char *warn);
extern void fatal(char *msg, int code);
extern void errorsummary(void);

/* glbsym.c */
extern void doglobal(T_storage stclass);
extern void dosymbol(list *root, entry *ptyp, T_storage stor, int fun);

/* locsym.c */
extern int dolocal(T_storage stclass);
extern void dolocenum(list *root,T_storage stor);

/* expr.c */
extern void expression(int comma);
extern int heir1(entry *item);

/* gen.c */
extern int getlabel(void);
extern char *makename(void);
extern void printlabel(int label);
extern void gnlabel(int nlab);
extern char outbyte(char c);
extern void outstr(char ptr[]);
extern void tab(void);
extern void nl(void);
extern void ol(char ptr[]);
extern void prefix(void);
extern void ot(char ptr[]);
extern void outdec(int number);
extern void store(entry *item);
extern void rvalue(entry *item);
extern void test(int label, int ft);

/* io.c */
extern void kill(void);
extern void finline(void);
extern char inchar(void);
extern char inbyte(void);
extern char gch(void);
extern char nch(void);
extern char ch(void);

/* lex.c */
extern void issemicolon(void);
extern void junk(void);
extern int endst(void);
extern void needbrack(char *str);
extern int alpha(char c);
extern int alnum(char c);
extern int sstreq(char *str1);
extern int streq(char *str1, char *str2);
extern int astreq(char *str1, char *str2, int len);
extern int match(char *lit);
extern void blanks(void);

/* preproc.c */
extern void doinclude();
extern void doasm(void);
extern void doundef(void);
extern void preprocess(void);
extern void defmac(char *mac);
extern void dodefine(void);
extern void delmac(char *mac);
extern void rmmac(void);

/* primary.c */
extern int primary(entry *item);
extern int dbltest(entry *val1, entry *val2);
extern void result(entry *val, entry *val2);
extern int number(int val[]);
extern int getint(void);
extern int pstr(int val[]);

/* stmt.c */
extern int statement(char *fname, int func);

/* sym.c */
extern void doconsts(list *root,T_storage stor);
extern int getsize(entry *item, int what);
extern int needsub(void);
extern int symname(char *sname);
extern void ungetname(char *name);
extern void multidef(char *sname);

/* while.c */
extern void addwhile(wsentry ws);
extern void delwhile(void);
extern wsentry *readwhile(void);
extern wsentry *findwhile(void);
extern wsentry *readswitch(void);
extern void addcase(int val);

/* vm.c */
#define underscore()	outbyte('_')
#define	comment()			outbyte(';')
extern void vmgetmem(entry *item);
extern void putmem(entry *item);
extern void vmgetloc(entry *item);
extern void vmputstk(entry *item);
extern void vmindirect(entry *item);
extern void vmswap(void);
extern void immed(void);
extern void vmadd(entry *itemp, entry *items);
extern void vmsub(entry *itemp, entry *items);
extern void vmmult(entry *itemp, entry *items);
extern void vmdiv(entry *itemp, entry *items);
extern void vmmod(entry *itemp, entry *items);
extern void vmor(entry *itemp, entry *items);
extern void vmxor(entry *itemp, entry *items);
extern void vmand(entry *itemp, entry *items);
extern void vmsr(entry *itemp, entry *items);
extern void vmsl(entry *itemp, entry *items);
extern void vmneg(entry *item);
extern void vmlneg(entry *item);
extern void vmcom(entry *item);
extern void vmbool(void);
extern void vminc(entry *item);
extern void vmdec(entry *item);
extern void vmeq(entry *itemp, entry *items);
extern void vmne(entry *itemp, entry *items);
extern void vmlt(entry *itemp, entry *items);
extern void vmle(entry *itemp, entry *items);
extern void vmgt(entry *itemp, entry *items);
extern void vmge(entry *itemp, entry *items);
extern void vmult(entry *itemp, entry *items);
extern void vmule(entry *itemp, entry *items);
extern void vmugt(entry *itemp, entry *items);
extern void vmuge(entry *itemp, entry *items);
extern void vmcall(char *sname);
extern void vmret(void);
extern void vmcallstk(void);
extern void vmjump(int label);
extern void vmtestjump(int label,int ft);
extern void vmpush(entry *item);
extern void vmpop(void);
extern void vmswapstk(void);
extern int vmmodstk(int newstkp);

/* code.c */
extern void asheader(void);
extern void astrailer(void);
extern void asprologue(char *str);
extern void astext(void);
extern void asdata(void);
extern void gaslint(void);
extern void gasrint(void);
extern void outcase(int typ,int value,int label);
extern void asargs(int d);
extern void asdumplits(void);
extern void asdumpglbs(void);
