#ifndef FVM_H_
#define FVM_H_

#include <stdint.h>

typedef int64_t fvm_word_t;
typedef double fvm_float_t;;

struct fvm;

// return non-zero if vm should be suspended
typedef void (*fvm_syscall_fn_t)(struct fvm* vm);

struct fvm {
  fvm_word_t sp;  // stack pointer
  fvm_word_t bp;  // base pointer
  fvm_word_t pc;  // programm counter
  fvm_word_t rv;  // return value
  fvm_syscall_fn_t *syscall_table;
};

enum fvm_op {
  FVM_OP_SP,
  FVM_OP_SSP,
  FVM_OP_BP,
  FVM_OP_SBP,
  FVM_OP_PC,
  FVM_OP_RV,
  FVM_OP_SRV,
  
  FVM_OP_IMM,
  
  FVM_OP_LD8,
  FVM_OP_LD16,
  FVM_OP_LD32,
  FVM_OP_LD,
  FVM_OP_ST8,
  FVM_OP_ST16,
  FVM_OP_ST32,
  FVM_OP_ST,
  
  FVM_OP_DUP,
  FVM_OP_POP,
  FVM_OP_SWAP,
  FVM_OP_OVER,
  FVM_OP_ROT,
  
  FVM_OP_ADD,
  FVM_OP_SUB,
  FVM_OP_DIV,
  FVM_OP_MUL,
  FVM_OP_MOD,
  
  FVM_OP_SHR,
  FVM_OP_SHL,
  FVM_OP_SAR,

  FVM_OP_AND,
  FVM_OP_OR,
  FVM_OP_NOT,

  FVM_OP_BITAND,
  FVM_OP_BITOR,
  FVM_OP_XOR,
  FVM_OP_INVERT,

  FVM_OP_GT,
  FVM_OP_LT,
  FVM_OP_GE,
  FVM_OP_LE,
  FVM_OP_EQ,
  FVM_OP_NEQ,

  FVM_OP_JMP,
  FVM_OP_JZ,
  FVM_OP_JNZ,

  FVM_OP_RET,
  FVM_OP_CALL,
  FVM_OP_SYSCALL,

  FVM_OP_FADD,
  FVM_OP_FSUB,
  FVM_OP_FMUL,
  FVM_OP_FDIV,

  FVM_OP_FGE,
  FVM_OP_FGT,
  FVM_OP_FLE,
  FVM_OP_FLT,
  FVM_OP_FEQ,
  FVM_OP_FNEQ,

  FVM_OP_FTI,
  FVM_OP_ITF,

  FVM_OP_EXIT,
};

int fvm_init(struct fvm *vm, void *code, int64_t stack_size);
int64_t fvm_execute(struct fvm *vm);

void fvm_push(struct fvm *vm, fvm_word_t value);
int64_t fvm_pop(struct fvm *vm);

fvm_float_t fvm_loadf(struct fvm *vm, fvm_word_t addr);
void fvm_storef(struct fvm *vm, fvm_word_t addr, fvm_float_t val);
  
fvm_word_t fvm_load(struct fvm *vm, fvm_word_t addr);
void fvm_store(struct fvm *vm, fvm_word_t addr, int64_t value);

int32_t fvm_load32(struct fvm *vm, fvm_word_t addr);
void fvm_store32(struct fvm *vm, fvm_word_t addr, int32_t value);

int16_t fvm_load16(struct fvm *vm, fvm_word_t addr);
void fvm_store16(struct fvm *vm, fvm_word_t addr, int16_t value);

int8_t fvm_load8(struct fvm *vm, fvm_word_t addr);
void fvm_store8(struct fvm *vm, fvm_word_t addr, int8_t value);

#endif
