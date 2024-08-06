%{
#include "parser.tab.h"
#include <string.h>

extern int lineCounter;

void notDefined();
%}

%%
\n              { lineCounter++; }

"="                 { return ASSIGN; }
"=="                { return EQ; }
"!="                { return DIF; }
">"                 { return BIGGER; }
"<"                 { return SMALLER; }
">="                { return BEQ; }
"<="                { return SEQ; }
"!"                 { return NEG; }

"("                 { return BRAL; }
")"                 { return BRAR; }
"{"                 { return HBRAL; }
"}"                 { return HBRAR; }
"["                 { return DBRAL; }
"]"                 { return DBRAR; }

"/"                 { return DIV; }
"+"                 { return ADD; }
"-"                 { return SUB; }
"*"                 { return MULT; }

","                 { return COMMA; }

"++"                { return INC; }
"--"                { return DEC; }

";"                 { return SEMICOLON; }

"int"               { return INT; }
"var"               { return VAR; }
"double"            { return DOUBLE; }
"string"            { return STR; }
"long"              { return LONG; }

"if"		    { return IF; }
"else"              { return ELSE; }
"for"		    { return FOR; }

".p"                { return PRINT; }
".m"		    { return MATH; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return VNAME; }
\"[a-zA-Z_][a-zA-Z0-9_ ]*\" {yylval.stringval = strdup(yytext); return STRINGV; }
[0-9]+              { yylval.ival = atoi(yytext); return NUM; }
[0-9]+\.[0-9]+      { yylval.dval = atof(yytext); return DNUM; }

[ \t]+            { /* Ignore whitespace */ }

.                   { notDefined(); return OTHER; }

%%

void notDefined(){
    printf("not defined.\n");
}

