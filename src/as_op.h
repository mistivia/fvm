#ifndef FVM_AS_OP_H_
#define FVM_AS_OP_H_

enum op {
    ADD, SUB, MUL, DIV, MOD, EQ,
    // place holder for the end of the table
    OPEND
};
typedef enum op Op;

Op str2op(const char *str);

#endif

