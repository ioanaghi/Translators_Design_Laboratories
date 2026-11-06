%{
#include <cstdlib>
#define NUMBER 256
int yylval;
%}

%option noyywrap

%%

[0-9]+ { yylval = atoi(yytext); return NUMBER; }
[ \t\r]+ ;
[+*/%()=\n-] return *yytext;
. return *yytext;

%%

