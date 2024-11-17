#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fvm.h"

int fvm_init(struct fvm *vm, void *code, int64_t stack_size) {
  void *stack_vm = malloc(stack_size);
  vm->sp = (fvm_word_t)(stack_vm + stack_size);
  vm->bp = vm->sp;
  vm->pc = (fvm_word_t)code;
  vm->rv = 0;
  return 1;
}

void fvm_push(struct fvm *vm, fvm_word_t val) {
  vm->sp -= sizeof(fvm_word_t);
  fvm_store(vm, vm->sp, val);
}

int64_t fvm_pop(struct fvm *vm) {
  int64_t r = fvm_load(vm, vm->sp);
  vm->sp += sizeof(fvm_word_t);
  return r;
}

void fvm_pushf(struct fvm *vm, fvm_float_t val) {
  assert(sizeof(fvm_word_t) >= sizeof(fvm_float_t));
  vm->sp -= sizeof(fvm_word_t);
  fvm_storef(vm, vm->sp, val);
}

fvm_float_t fvm_popf(struct fvm *vm) {
  assert(sizeof(fvm_word_t) >= sizeof(fvm_float_t));
  fvm_float_t r = fvm_loadf(vm, vm->sp);
  vm->sp += sizeof(fvm_word_t);
  return r;
}

fvm_float_t fvm_loadf(struct fvm *vm, fvm_word_t addr) {
  return *(fvm_float_t*)addr;
}

void fvm_storef(struct fvm *vm, fvm_word_t addr, fvm_float_t val) {
  *(fvm_float_t*)addr = val;
}

fvm_word_t fvm_load(struct fvm *vm, fvm_word_t addr) {
  return *(fvm_word_t*)addr;
}
void fvm_store(struct fvm *vm, fvm_word_t addr, int64_t value) {
  *(fvm_word_t*)addr = value;
}

int32_t fvm_load32(struct fvm *vm, fvm_word_t addr) {
  return *(int32_t*)addr;
}
void fvm_store32(struct fvm *vm, fvm_word_t addr, int32_t value) {
  *(int32_t*)addr = value;
}

int16_t fvm_load16(struct fvm *vm, fvm_word_t addr) {
  return *(int16_t*)addr;
}
void fvm_store16(struct fvm *vm, fvm_word_t addr, int16_t value) {
  *(int16_t*)addr = value;
}

int8_t fvm_load8(struct fvm *vm, fvm_word_t addr) {
    return *(int8_t*)addr;
}
void fvm_store8(struct fvm *vm, fvm_word_t addr, int8_t value) {
  *(int8_t*)addr = value;
}


int64_t fvm_execute(struct fvm *vm) {
  fvm_word_t a, b, c;
  fvm_float_t x, y, z;
  while (1) {
    enum fvm_op op = (enum fvm_op)(uint8_t)fvm_load8(vm, vm->pc);
    switch (op) {
    case FVM_OP_SP:
      a = vm->sp;
      fvm_push(vm, a);
      vm->pc++;
      break;
    case FVM_OP_SSP:
      vm->sp = fvm_pop(vm);
      vm->pc++;
      break;
    case FVM_OP_BP:
      fvm_push(vm, vm->bp);
      vm->pc++;
      break;
    case FVM_OP_SBP:
      vm->bp = fvm_pop(vm);
      vm->pc++;
      break;
    case FVM_OP_RV:
      a = vm->rv;
      fvm_push(vm, a);
      vm->pc++;
      break;
    case FVM_OP_SRV:
      vm->rv = fvm_pop(vm);
      vm->pc++;
      break;
    case FVM_OP_PC:
      fvm_push(vm, vm->pc);
      vm->pc++;
      break;
    case FVM_OP_IMM:
      fvm_push(vm, fvm_load(vm, vm->pc + 1));
      vm->pc += sizeof(fvm_word_t) + 1;
      break;
    case FVM_OP_LD:
      fvm_push(vm, fvm_load(vm, fvm_pop(vm)));
      vm->pc++;
      break;
    case FVM_OP_LD32:
      fvm_push(vm, fvm_load32(vm, fvm_pop(vm)));
      vm->pc++;
      break;
    case FVM_OP_LD16:
      fvm_push(vm, fvm_load16(vm, fvm_pop(vm)));
      vm->pc++;
      break;
    case FVM_OP_LD8:
      fvm_push(vm, fvm_load8(vm, fvm_pop(vm)));
      vm->pc++;
      break;
    case FVM_OP_ST:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_store(vm, b, a);
      vm->pc++;
      break;
    case FVM_OP_ST32:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_store32(vm, b, a);
      vm->pc++;
      break;
    case FVM_OP_ST16:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_store16(vm, b, a);
      vm->pc++;
      break;
    case FVM_OP_ST8:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_store8(vm, b, a);
      vm->pc++;
      break;
    case FVM_OP_DUP:
      a = fvm_pop(vm);
      fvm_push(vm, a);
      fvm_push(vm, a);
      vm->pc++;
      break;
    case FVM_OP_POP:
      fvm_pop(vm);
      vm->pc++;
      break;
    case FVM_OP_SWAP:
      a = fvm_pop(vm);
      b = fvm_pop(vm);
      fvm_push(vm, a);
      fvm_push(vm, b);
      vm->pc++;
      break;
    case FVM_OP_OVER:
      a = fvm_pop(vm);
      b = fvm_pop(vm);
      fvm_push(vm, b);
      fvm_push(vm, a);
      fvm_push(vm, b);
      vm->pc++;
      break;
    case FVM_OP_ROT:
      a = fvm_pop(vm);
      b = fvm_pop(vm);
      c = fvm_pop(vm);
      fvm_push(vm, b);
      fvm_push(vm, a);
      fvm_push(vm, c);
      vm->pc++;
      break;
    case FVM_OP_ADD:
      a = fvm_pop(vm);
      b = fvm_pop(vm);
      fvm_push(vm, a + b);
      vm->pc++;
      break;
    case FVM_OP_SUB:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a - b);
      vm->pc++;
      break;
    case FVM_OP_MUL:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a * b);
      vm->pc++;
      break;
    case FVM_OP_DIV:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a / b);
      vm->pc++;
      break;
    case FVM_OP_MOD:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a % b);
      vm->pc++;
      break;
    case FVM_OP_SHR:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, (int64_t)(((uint64_t)a) >> b));
      vm->pc++;
      break;
    case FVM_OP_SHL:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a << b);
      vm->pc++;
      break;
    case FVM_OP_SAR:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a >> b);
      vm->pc++;
      break;
    case FVM_OP_AND:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a && b);
      vm->pc++;
      break;
    case FVM_OP_OR:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a || b);
      vm->pc++;
      break;
    case FVM_OP_NOT:
      a = fvm_pop(vm);
      fvm_push(vm, !a);
      vm->pc++;
      break;
    case FVM_OP_BITAND:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a & b);
      vm->pc++;
      break;
    case FVM_OP_BITOR:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a | b);
      vm->pc++;
      break;
    case FVM_OP_XOR:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a ^ b);
      vm->pc++;
      break;
    case FVM_OP_INVERT:
      a = fvm_pop(vm);
      fvm_push(vm, ~a);
      vm->pc++;
      break;
    case FVM_OP_GE:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a >= b);
      vm->pc++;
      break;
    case FVM_OP_LE:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a <= b);
      vm->pc++;
      break;
    case FVM_OP_GT:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a > b);
      vm->pc++;
      break;
    case FVM_OP_LT:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a < b);
      vm->pc++;
      break;
    case FVM_OP_EQ:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a == b);
      vm->pc++;
      break;
    case FVM_OP_NEQ:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      fvm_push(vm, a != b);
      vm->pc++;
      break;
    case FVM_OP_JMP:
      vm->pc = fvm_pop(vm);
      break;
    case FVM_OP_JZ:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      if (a) {
	vm->pc++;
	break;
      } else {
	vm->pc = b;
	break;
      }
    case FVM_OP_JNZ:
      b = fvm_pop(vm);
      a = fvm_pop(vm);
      if (a) {
	vm->pc = b;
	break;
      } else {
	vm->pc++;
	break;
      }
    case FVM_OP_CALL:
      a = fvm_pop(vm);
      fvm_push(vm, vm->pc);
      vm->pc = a;
      break;
    case FVM_OP_RET:
      a = fvm_pop(vm);
      vm->pc = a;
      break;
    case FVM_OP_SYSCALL:
      a = fvm_pop(vm);
      vm->pc++;
      (*(vm->syscall_table[a]))(vm);
      break;
    case FVM_OP_FADD:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_pushf(vm, x + y);
      vm->pc++;
      break;
    case FVM_OP_FSUB:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_pushf(vm, x - y);
      vm->pc++;
      break;
    case FVM_OP_FMUL:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_pushf(vm, x * y);
      vm->pc++;
      break;
    case FVM_OP_FDIV:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_pushf(vm, x / y);
      vm->pc++;
      break;
    case FVM_OP_FGE:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_push(vm, x >= y);
      vm->pc++;
      break;
    case FVM_OP_FGT:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_push(vm, x > y);
      vm->pc++;
      break;
    case FVM_OP_FLE:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_push(vm, x <= y);
      vm->pc++;
      break;
    case FVM_OP_FLT:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_push(vm, x < y);
      vm->pc++;
      break;
    case FVM_OP_FEQ:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_push(vm, x == y);
      vm->pc++;
      break;
    case FVM_OP_FNEQ:
      y = fvm_popf(vm);
      x = fvm_popf(vm);
      fvm_push(vm, x != y);
      vm->pc++;
      break;
    case FVM_OP_FTI:
      x = fvm_popf(vm);
      fvm_push(vm, (fvm_word_t)x);
      vm->pc++;
      break;
    case FVM_OP_ITF:
      a = fvm_pop(vm);
      fvm_pushf(vm, (fvm_float_t)x);
      vm->pc++;
      break;
    case FVM_OP_EXIT:
      return vm->rv;
    default:
      fprintf(stderr, "unknown opcode.\n");
      break; 
    }
  }
  return -1;
}
