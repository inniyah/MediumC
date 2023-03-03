/**********************************************************
* MediumC PIC C Crosscompiler.  version: beta
*
* file:   error.h
* date:   January 1999
*
* author: Janusz J. Mlodzianowski, fizjm@univ.gda.pl
**********************************************************/
File PREPROC.C:
"too many nested includes"
"Could not open include file"
"incorrect delimiter"
"no matching #if..."
"missing quote"
"missing apostrophe"
#define TOOLONG "line too long"
"Duplicate define"

File LEX.C:
"missing semicolon"
"missing bracket"

File PRIMARY.C:
"sizeof local static"
"sizeof undeclared variable"
"sizeof only on type or variable"
"undeclared variable"
"invalid expression"
"string space exhausted"
"wrong number of arguments in call"

File EXPR.C:
"must be lvalue"
"missing colon"
"illegal address"
"must be lvalue"
"must be lvalue"
"can't subscript"
"can't subscript"

File FUNCTION.C:
"illegal function or declaration"
"missing open paren"
"declaration syntax error"

File MAIN.C:
"missing closing bracket"
"missing costam"
"Duplicate define"
"Duplicate define"
"declaration syntax error"

File STMT.C:
error("function requires compound statement"
error("missing while"
error("bad case label"
error("missing colon"
error("no active switch"
error("missing colon"
error("no active switch"
error("Function is declared as void"
error("Function should return a value"

File SYM.C:
error("illegal declaration"
error("must be constant"
"negative size illegal"
"global symbol table overflow"
"local symbol table overflow"
"illegal symbol name"
"already defined"

File WHILE.C:
"too many active whiles"
"no active do/for/while/switch"
"no active do/for/while"
"too many case labels"
