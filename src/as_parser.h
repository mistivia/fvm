#ifndef FVM_AS_PARSER_H_
#define FVM_AS_PARSER_H_

#include "as_tokenizer.h"

#include "as_op.h"

struct arg {
    int64_t ival;
    double fval;
};
typedef struct arg Arg;

struct instr {
    Op op;
    Arg* arg;
    const char* tagName;
};
typedef struct instr Instr;

struct label {
    const char* name;
};
typedef struct label Label;

struct stmt {
    Label* label;
    Instr* instr;
};
typedef struct stmt Stmt;

struct stmts {
    Stmt** stmts;
};
typedef struct stmts Stmts;

struct prog {
    Stmts *stmts;
};
typedef struct prog Prog;

Prog* parseProg(TokenStream *ts);
Stmt* parseStmt(TokenStream *ts);
Stmts* parseStmts(TokenStream *ts);
Instr* parseInstr(TokenStream *ts);
Label* parseLabel(TokenStream *ts);
Op parseOp(TokenStream *ts);

#endif
