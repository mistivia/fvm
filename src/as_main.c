#include <stdio.h>

#include "as_tokenizer.h"

// AST
// ===
//
// <prog> ::= <stmts>
// <stmts> ::= <stmt> <newline> | <stmt> <stmts> <newline>
// <stmt> ::= <tag> <instr> | <instr> | <tag>
// <instr> ::= <op> | <op> <arg> | <op> <label>
// <tag> ::= <label> :
// <op> ::= add | sub | mul | div | mod | eq

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: fvm-as <inputfile>\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    TokenStream* ts = makeTokenStream(fp);
    return 0;
}

