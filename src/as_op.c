#include "as_op.h"

#include <string.h>

struct opTableEntry{
    Op op;
    const char* name;
};
typedef struct opTableEntry OpTableEntry;

OpTableEntry opTable[] = {
    {ADD, "add"},
    {SUB, "sub"},
    {MUL, "mul"},
    {DIV, "div"},
    {MOD, "mod"},
    {EQ, "eq"},
    {OPEND, NULL}
};

Op str2op(const char* str) {
    for (int i = 0; opTable[i].name != NULL; i++) {
        if (strcmp(opTable[i].name, str) == 0) {
            return opTable[i].op;
        }
    }
    return OPEND;
}
