#include "as_parser.h"

#include <stdlib.h>

// BNF
// ===
//
// <prog> ::= <stmts>
// <stmts> ::= <stmt> <newline> | <stmt> <stmts>
// <stmt> ::= <tag> <instr> | <instr> | <tag>
// <instr> ::= <op> | <op> <arg> | <op> <label>
// <tag> ::= <label> :
// <op> ::= add | sub | mul | div | mod | eq


Prog* parseProg(TokenStream *ts) {
    Prog *p = malloc(sizeof(Prog));
    p->stmts = parseStmts(ts);
    return p;
}

Stmts* parseStmts(TokenStream *ts) {
    Stmts *ss = malloc(sizeof(Stmts));
    ss->stmts = malloc(sizeof(Stmt*));
    ss->stmts[0] = NULL;
    int capacity = 0;
    int len = 0;
    while (peekToken(ts)->type != ENDOFFILE) {
        Stmt *s = parseStmt(ts);
        if (len == capacity) {
            capacity = capacity * 2 + 1;
            ss->stmts = realloc(ss->stmts, sizeof(Stmt*) * capacity);
        }
        ss->stmts[len] = s;
        len++;
    }
    ss->stmts[len] = NULL;
    return ss;
}

Stmt* parseStmt(TokenStream *ts) {
    Token *t = peekToken(ts);
    if (t->type == LABEL) {
        Label *l = parseLabel(ts);
        t = peekToken(ts);
        if (t->type == COLON) {
            nextToken(ts);
            return (Stmt*)l;
        } else {
            Instr *i = parseInstr(ts);
            Stmt *s = malloc(sizeof(Stmt));
            s->tag = l;
            s->instr = i;
            return s;
        }
    } else {
        Instr *i = parseInstr(ts);
        Stmt *s = malloc(sizeof(Stmt));
        s->tag = NULL;
        s->instr = i;
        return s;
    }
}

Instr* parseInstr(TokenStream *ts) {
    Token *t = nextToken(ts);
    Instr *i = malloc(sizeof(Instr));
    i->labelName = NULL;
    if (t->type == OP) {
        i->op = parseOp(ts);
        t = peekToken(ts);
        if (t->type == ARG) {
            Arg *a = malloc(sizeof(Arg));
            a->ival = t->ival;
            a->fval = t->fval;
            a->sval = t->sval;
            i->arg = a;
            nextToken(ts);
        } else if (t->type == LABEL) {
            i->labelName = t->sval;
            nextToken(ts);
        }
    }
    return i;
}
