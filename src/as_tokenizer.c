#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "as_tokenizer.h"

int InputStream_nextChar(InputStream *s) {
    if (s->cursor == -1) {
        return EOF;
    }
    if (s->buf_pos == s->cursor) {
        size_t n = fread(s->buf, 1, INPUT_STREAM_BUF_SIZE, s->fp);
        if (n == 0) {
            s->cursor = -1;
            return EOF;
        }
        s->buf_pos = n;
        s->cursor = 0;
    }
    int c = s->buf[s->cursor];
    s->cursor++;
    if (c == '\n') {
        s->line++;
        s->col = 1;
    } else {
        s->col++;
    }
    return c;
}

int InputStream_peekChar(InputStream *s) {
    if (s->cursor == -1) {
        return EOF;
    }
    if (s->buf_pos == s->cursor) {
        size_t n = fread(s->buf, 1, INPUT_STREAM_BUF_SIZE, s->fp);
        if (n == 0) {
            return EOF;
        }
        s->buf_pos = n;
        s->cursor = 0;
    }
    return s->buf[s->cursor];
}

char* ops[] = {
    "add", "sub", "mul", "div", "mod", "eq"
};

int isOp(const char* str) {
    for (int i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
        if (strcmp(ops[i], str) == 0) {
            return 1;
        }
    }
    return 0;
}

int isStartOfIndentifier(int c) {
    if (c >= 'a' && c <= 'z') {
        return 1;
    }
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    if (c == '_') {
        return 1;
    }
    return 0;
}

int isPartOfIndentifier(int c) {
    if (isStartOfIndentifier(c)) {
        return 1;
    }
    if (c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

Token nextTokenImpl(InputStream *s) {
    int c;
    while (1) {
        c = InputStream_peekChar(s);
        if (c == EOF) {
            break;
        }
        if (c == '\n') {
            InputStream_nextChar(s);
            Token t = (Token){.type = NEWLINE, .line = s->line, .col = s->col};
            return t;
        }
        if (c == ':') {
            InputStream_nextChar(s);
            return (Token){.type = COLON, .line = s->line, .col = s->col};
        }
        if (c == ' ' || c == '\t') {
            InputStream_nextChar(s);
            continue;
        }
        if (c >= '0' && c <= '9') {
            int64_t ival = 0;
            while (1) {
                InputStream_nextChar(s);
                ival = ival * 10 + (c - '0');
                c = InputStream_peekChar(s);
                if (c < '0' || c > '9') {
                    break;
                }
            } 
            return (Token){.type = ARG, .ival = ival, .line = s->line, .col = s->col};
        }
        if (isStartOfIndentifier(c)) {
            size_t line = s->line;
            size_t col = s->col;
            char *sval = malloc(256);
            size_t i = 0;
            while(1) {
                if (i >= 255) {
                    fprintf(stderr, "error: identifier too long\n");
                    exit(1);
                }
                InputStream_nextChar(s);
                sval[i++] = c;
                c = InputStream_peekChar(s);
                if (!isPartOfIndentifier(c)) {
                    break;
                }
            }
            sval[i] = '\0';
            if (isOp(sval)) {
                return (Token){.type = OP, .sval = sval, .line = line, .col = col};
            }
            return (Token){.type = LABEL, .sval = sval, .line = line, .col = col};
        }
        fprintf(stderr, "error: invalid character %c at line %d, col %d\n", c, s->line, s->col);
    }
    // end of file
    return (Token){.type = ENDOFFILE};
}

Token *nextToken(TokenStream *ts) {
    if (ts->buf != NULL) {
        Token *t = ts->buf;
        ts->buf = NULL;
        return t;
    }
    Token *t = malloc(sizeof(Token));
    *t = nextTokenImpl(ts->s);
    return t;
}

Token *peekToken(TokenStream *ts) {
    if (ts->buf != NULL) {
        return ts->buf;
    }
    ts->buf = malloc(sizeof(Token));
    *(ts->buf) = nextTokenImpl(ts->s);
    return ts->buf;
}

void printToken(Token *t) {
    switch (t->type) {
        case OP:
            printf("OP: %s, line: %d, col: %d\n", t->sval, t->line, t->col);
            break;
        case ARG:
            printf("ARG: %ld, line: %d, col: %d\n", t->ival, t->line, t->col);
            break;
        case LABEL:
            printf("LABEL: %s, line: %d, col: %d\n", t->sval, t->line, t->col);
            break;
        case COLON:
            printf("COLON\n");
            break;
        case NEWLINE:
            printf("NEWLINE\n");
            break;
        case ENDOFFILE:
            printf("ENDOFFILE\n");
            break;
    }
}

TokenStream* makeTokenStream(FILE* fp) {
    InputStream *s = malloc(sizeof(InputStream));
    s->fp = fp;
    s->buf = malloc(INPUT_STREAM_BUF_SIZE);
    s->buf_pos = 0;
    s->cursor = 0;
    s->line = 1;
    s->col = 1;
    TokenStream *ts = malloc(sizeof(TokenStream));
    ts->s = s;
    ts->buf = NULL;
    return ts;
}
