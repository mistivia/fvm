#ifndef FMV_AS_TOKENIZER_H_
#define FMV_AS_TOKENIZER_H_

#include <stdint.h>
#include <stdio.h>

typedef enum {
    OP, ARG, TAG, COLON, NEWLINE, ENDOFFILE
} TokenType;

typedef struct {
    TokenType type;
    int line;
    int col;
    char *sval;
    int64_t ival;
    double fval;
} Token;

#define INPUT_STREAM_BUF_SIZE 1024

typedef struct {
    FILE *fp;
    char *buf;
    int buf_pos;
    int cursor;
    int line;
    int col;
} InputStream;

typedef struct {
    Token* buf;
    InputStream *s;
} TokenStream;

Token *nextToken(TokenStream *ts);
Token *peekToken(TokenStream *ts);
void printToken(Token *t);
TokenStream* makeTokenStream(FILE* fp);

#endif // FMV_AS_TOKENIZER_H_
