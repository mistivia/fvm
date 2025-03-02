#include "as_tokenizer.h"

// BNF
// ===
//
// <prog> ::= <stmts>
// <stmts> ::= <stmt> <newline> | <stmt> <stmts>
// <stmt> ::= <tag> <instr> | <instr> | <tag>
// <instr> ::= <op> | <op> <arg> | <op> <label>
// <tag> ::= <label> :
// <op> ::= add | sub | mul | div | mod | eq

enum op {
    ADD, SUB, MUL, DIV, MOD, EQ
};
typedef enum op Op;

struct arg {
    int64_t ival;
    double fval;
    const char *sval;
};
typedef struct arg Arg;

struct instr {
    Op op;
    Arg* arg;
    const char* labelName;
};
typedef struct instr Instr;

struct label {
    const char* name;
};
typedef struct label Label;

struct stmt {
    Label* tag;
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

