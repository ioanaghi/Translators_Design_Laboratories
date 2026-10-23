%{
#include <stdlib.h>
#define INTEGER 256
extern int yylval;        /* defined in main_bnf_todo.c */
%}

%option noyywrap

%%

[0-9]+       { yylval = atoi(yytext); return INTEGER; }
[+()\n]      { return yytext[0]; }
[ \t]+       ;    /* skip whitespace */
.            { return (unsigned char) yytext[0]; } /* pass invalid chars to parser */

%%
