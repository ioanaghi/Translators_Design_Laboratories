/* 
 *
 *     Element    eBNF         Explanation
 *     Option     [X]          if (symbol in FIRST(X)) X();
 *     Closure    X+           do { X(); } while (symbol in FIRST(X));
 *                X*           while (symbol in FIRST(X)) X();
 *     List       X || D       X(); while (symbol in FIRST(D)) { D(); X(); }
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

#define INTEGER 256

//Lexer stuff
int yylval;
int symbol;

void next_symbol(void) { symbol = yylex(); }

void Z(void);
int  E(void);
int  T(void);

/*
 * Z ::= E .
 *
 * The start symbol just wraps E. Print the result here. */

void Z(void) {
    int value = E();

    /* A complete expression occupies one input line.  Accept EOF as the
       terminator too, so the last line need not end with a newline. */
    if (symbol != '\n' && symbol != 0) {
        if (symbol == INTEGER)
            fprintf(stderr, "Syntax error: unexpected integer after expression\n");
        else
            fprintf(stderr, "Syntax error: unexpected character '%c' after expression\n",
                    symbol);
        exit(EXIT_FAILURE);
    }

    printf("%d\n", value);
    if (symbol == '\n') next_symbol();
}

/* E ::= T ( '+' T )*               (same as E ::= T || '+')
 * 
 * List schema  X || D  with  X = T  and  D = '+':
 *     T();
 *     while (symbol in FIRST('+')) {   // i.e. symbol == '+'
 *         '+' ();                      // consume the plus
 *         T();                         // parse the next term
 *     }
 * Accumulate the sum as you go, return the result.
 */
int E(void) {
    int value = T();

    /* The list separator is '+'.  Each following T contributes to the sum. */
    while (symbol == '+') {
        next_symbol();
        value += T();
    }

    return value;
}

/* T ::= i | '(' E ')' .
 *
 * Pure alternation. The two branches start with disjoint
 * terminals (INTEGER vs '('), so one token of lookahead picks
 * the branch unambiguously.
 */
int T(void) {
    if (symbol == INTEGER) {
        int value = yylval;
        next_symbol();
        return value;
    }
    if (symbol == '(') {
        int value;

        next_symbol();
        value = E();
        if (symbol != ')') {
            fprintf(stderr, "Syntax error: expected ')'\n");
            exit(EXIT_FAILURE);
        }
        next_symbol();
        return value;
    }

    if (symbol == 0 || symbol == '\n')
        fprintf(stderr, "Syntax error: expected an integer or '('");
    else
        fprintf(stderr, "Syntax error: expected an integer or '(', got '%c'", symbol);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

int main(void) {
    next_symbol();
    while (symbol == '\n') next_symbol(); //skip blank lines
    while (symbol != 0) {
        Z(); //evaluate one line at a time
        while (symbol == '\n') next_symbol(); //skip the trailing newlines
    }
    return 0;
}
