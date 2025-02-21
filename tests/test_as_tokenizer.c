#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <assert.h>

#include "as_tokenizer.h"

char *inputBuffer = 
    "start:\n"
    "    add 1\n"
    "    sub start\n"
    "    div\n"
    "    eq\n";

char *expectedOutput = 
    "LABEL: start, line: 1, col: 1\n"
    "COLON\n"
    "NEWLINE\n"
    "OP: add, line: 2, col: 5\n"
    "ARG: 1, line: 2, col: 10\n"
    "NEWLINE\n"
    "OP: sub, line: 3, col: 5\n"
    "LABEL: start, line: 3, col: 9\n"
    "NEWLINE\n"
    "OP: div, line: 4, col: 5\n"
    "NEWLINE\n"
    "OP: eq, line: 5, col: 5\n"
    "NEWLINE\n";

int main(int argc, char** argv) {
    printf("[TEST] assembler tokenizer\n");
    // make a memory buffer to FILE*
    FILE *fp = fmemopen(inputBuffer, strlen(inputBuffer), "r");
    TokenStream* ts = makeTokenStream(fp);
    
    char *outputBuffer = malloc(10240);
    // redirect stdout to a file
    FILE *out = fmemopen(outputBuffer, 10240, "w");
    FILE *origin_stdout = stdout;
    stdout = out;
    while (peekToken(ts)->type != ENDOFFILE) {
        printToken(peekToken(ts));
        nextToken(ts);
    }
    fclose(out);
    stdout = origin_stdout;
    // compare outputBuffer with expectedOutput
    assert(strcmp(outputBuffer, expectedOutput) == 0);
    printf("[PASS] assembler tokenizer\n");
    return 0;
}


