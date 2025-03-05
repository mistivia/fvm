#include "as_parser.h"

#include <stdlib.h>

// BNF
// ===
//
// <prog> ::= <stmts>
// <stmts> ::= <stmt> <newline> | <stmt> <stmts>
// <stmt> ::= <label> <instr> | <instr> | <label>
// <instr> ::= <op> | <op> <arg> | <op> <tag>
// <label> ::= <tag> :
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

Label* parseLabel(TokenStream *ts) {
    Token *t = nextToken(ts);
    if (t->type != TAG) {
        fprintf(stderr, "%d:%d Expect LABEL.\n", t->line, t->col);
        exit(-1);
    }
    Label *l = malloc(sizeof(Label));
    l->name = t->sval;
    t = nextToken(ts);
    if (t->type != COLON) {
        fprintf(stderr, "%d:%d Expect COLON.\n", t->line, t->col);
        exit(-1);
    }
    return l;
}

Stmt* parseStmt(TokenStream *ts) {
    Token *t = peekToken(ts);
    if (t->type == TAG) {
        Label *l = parseLabel(ts);
        t = peekToken(ts);
        if (t->type == COLON) {
            nextToken(ts);
            return (Stmt*)l;
        } else {
            Instr *i = parseInstr(ts);
            Stmt *s = malloc(sizeof(Stmt));
            s->label = l;
            s->instr = i;
            return s;
        }
    } else {
        Instr *i = parseInstr(ts);
        Stmt *s = malloc(sizeof(Stmt));
        s->label = NULL;
        s->instr = i;
        return s;
    }
}

// parse op
Op parseOp(TokenStream *ts) {
    Token *t = nextToken(ts);
    Op op;
    if (t->type == OP) {
        op = str2op(t->sval);
        if (op == OPEND) {
            fprintf(stderr, "%d:%d Invalid OP.\n", t->line, t->col);
            exit(-1);
        }
    } else {
        fprintf(stderr, "%d:%d Expect OP.\n", t->line, t->col);
        exit(-1);
    }
    return op;
}

Instr* parseInstr(TokenStream *ts) {
    Token *t = nextToken(ts);
    Instr *i = malloc(sizeof(Instr));
    i->tagName = NULL;
    if (t->type == OP) {
        i->op = parseOp(ts);
        t = peekToken(ts);
        if (t->type == ARG) {
            Arg *a = malloc(sizeof(Arg));
            a->ival = t->ival;
            a->fval = t->fval;
            i->arg = a;
            nextToken(ts);
        } else if (t->type == TAG) {
            i->tagName = t->sval;
            nextToken(ts);
        }
    }
    return i;
}
