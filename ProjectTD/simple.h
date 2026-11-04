#ifndef SIMPLE_H
#define SIMPLE_H

enum {
    INTEGER = 258, VARIABLE, WHILE, IF, PRINT,
    IFX, ELSE, GE, LE, EQ, NE, UMINUS
};

extern int yylval;
int yylex(void);

#endif
