/* 
 * Recursive-descent parser for the AST problem.
 *
 * The grammar:
 *
 *     E -> T '|' E | T          (alternation, right-recursive)
 *     T -> F T   | F            (concatenation, no operator)
 *     F -> P*    | P            
 *     P -> (E)   | letter
 *
 * You produce an AST where each operator becomes a node:
 *
 *     '|'  (alternation)
 *     '&'  (concatenation -- we invent this symbol since
 *                   the regex has no visible operator for it)
 *     '*'  (star, uses left child only)
 *     a..z leaves  (op holds the letter, children are NULL)
 *
 * The invariant that makes the whole thing work:
 *     Every parsing function returns with `symbol` pointing at
 *     the FIRST TOKEN AFTER whatever it consumed.
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

#define LETTER 256

// Lexer stuff
char yylval;              // the lexer writes the current letter here 
int  symbol;              // one token of lookahead                    

void next_symbol(void) { symbol = yylex(); }

// AST node 
typedef struct Node {
    char op;
    struct Node *left;
    struct Node *right;
} Node;

Node* makeNode(char op, Node* left, Node* right) {
    Node* n = malloc(sizeof(Node));
    if (!n) { fprintf(stderr, "out of memory\n"); exit(1); }
    n->op = op;
    n->left = left;
    n->right = right;
    return n;
}

Node* E(void);
Node* T(void);
Node* F(void);
Node* P(void);

/* 
 * TODO 1 -- P -> (E) | letter
 * 
 * - If symbol is '(':
 *       consume it, call E() to parse the inner expression,
 *       check that the next symbol is ')' and consume it,
 *       return the inner node (parens leave NO node in the AST).
 * - If symbol is LETTER:
 *       make a leaf node whose op is the current `yylval`,
 *       consume the letter, return the leaf.
 * - Otherwise: print an error and exit.
 */
Node* P(void) {
    if (symbol == '(') {
        next_symbol();
        Node* inner = E();
        if (symbol != ')') {
            fprintf(stderr, "syntax error: expected ')', got %s\n",
                    symbol == 0 ? "end of input" : "another token");
            exit(1);
        }
        next_symbol();
        return inner;
    }

    if (symbol == LETTER) {
        Node* leaf = makeNode(yylval, NULL, NULL);
        next_symbol();
        return leaf;
    }

    fprintf(stderr, "syntax error: expected '(' or a letter\n");
    exit(1);
}

/* 
 * TODO 2 -- F -> P* | P
 * 
 * - Parse a P.
 * - If the next symbol is '*':
 *      consume it and wrap the P in a '*' node, 
 *      star node will be the root, left = the P, right = NULL.
 * - Return the (possibly wrapped) node.
 */
Node* F(void) {
    Node* node = P();
    if (symbol == '*') {
        next_symbol();
        node = makeNode('*', node, NULL);
    }
    return node;
}

/* 
 * TODO 3 -- T -> F T | F     (concatenation, no visible operator)
 * 
 * Trick: there is no concatenation symbol. You detect "another F
 * is coming" by LOOKAHEAD -- only '(' and LETTER can start an F.
 *
 * - If symbol is '(' or LETTER:
 *     return a node with op='&', left=F(), right=T()  (right-recursive!)
 *   Otherwise:
 *       return left.
 *
 * Right-recursion matters: for "abc" the tree must be
 *     & (a, & (b, c))     NOT     & (& (a, b), c).
 */
Node* T(void) {
    Node* left = F();
    if (symbol == '(' || symbol == LETTER) {
        return makeNode('&', left, T());
    }
    return left;
}

/* 
 * TODO 4 -- E -> T '|' E | T
 * 
 * - Parse a T into `left`.
 * - If symbol is '|':
 *       consume it, build a '|' node with left=T and right=E().
 *   Otherwise:
 *       return left.
 */
Node* E(void) {
    Node* left = T();
    if (symbol == '|') {
        next_symbol();
        Node* right = E();
        return makeNode('|', left, right);
    }

    return left;
}

// ASCII tree layout information: the subtree width and its root position.
typedef struct Layout {
    int width;
    int root_x;
} Layout;

Layout getLayout(Node* node) {
    if (!node) return (Layout){0, 0};

    Layout left = getLayout(node->left);
    Layout right = getLayout(node->right);

    if (node->left && node->right) {
        int right_offset = left.width + 4;
        int left_root = left.root_x;
        int right_root = right_offset + right.root_x;
        return (Layout){right_offset + right.width,
                        (left_root + right_root) / 2};
    }
    if (node->left) return left;
    if (node->right) return right;
    return (Layout){1, 0};
}

int treeDepth(Node* node) {
    if (!node) return 0;
    int left = treeDepth(node->left);
    int right = treeDepth(node->right);
    return 1 + (left > right ? left : right);
}

void drawTree(Node* node, char** rows, int origin, int depth) {
    if (!node) return;

    Layout node_layout = getLayout(node);
    int x = origin + node_layout.root_x;
    int row = depth * 2;
    rows[row][x] = node->op;

    if (node->left && node->right) {
        Layout left = getLayout(node->left);
        int right_origin = origin + left.width + 4;
        Layout right = getLayout(node->right);
        int left_x = origin + left.root_x;
        int right_x = right_origin + right.root_x;
        rows[row + 1][(x + left_x) / 2] = '/';
        rows[row + 1][(x + right_x) / 2] = '\\';
        drawTree(node->left, rows, origin, depth + 1);
        drawTree(node->right, rows, right_origin, depth + 1);
    } else if (node->left) {
        rows[row + 1][x] = '|';
        drawTree(node->left, rows, origin, depth + 1);
    } else if (node->right) {
        rows[row + 1][x] = '|';
        drawTree(node->right, rows, origin, depth + 1);
    }
}

// Pretty-print the AST top-down with branches, like a conventional tree diagram.
void printTree(Node* node, int indent) {
    (void)indent;
    if (!node) return;

    Layout layout = getLayout(node);
    int row_count = treeDepth(node) * 2 - 1;
    char** rows = malloc((size_t)row_count * sizeof(char*));
    if (!rows) { fprintf(stderr, "out of memory\n"); exit(1); }
    for (int i = 0; i < row_count; i++) {
        rows[i] = malloc((size_t)layout.width + 1);
        if (!rows[i]) { fprintf(stderr, "out of memory\n"); exit(1); }
        for (int j = 0; j < layout.width; j++) rows[i][j] = ' ';
        rows[i][layout.width] = '\0';
    }

    drawTree(node, rows, 0, 0);
    for (int i = 0; i < row_count; i++) {
        int end = layout.width;
        while (end > 0 && rows[i][end - 1] == ' ') end--;
        rows[i][end] = '\0';
        puts(rows[i]);
        free(rows[i]);
    }
    free(rows);
}

// Main already created
int main(void) {
    next_symbol();
    while (symbol == '\n') next_symbol(); //skip blank lines
    while (symbol != 0) {
        Node* root = E();
        printf("Parse tree:\n");
        printTree(root, 0);
        while (symbol == '\n') next_symbol(); //skip trailing newlines
    }
    return 0;
}
