%{
#include <stdlib.h>
#include "simple.h"
%}

%%
"while"   { return WHILE; }
[0-9]+    { yylval = atoi(yytext); return INTEGER; }
[()\n+]         return yytext[0];
%%
