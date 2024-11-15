#include <assert.h>
#include <stdio.h>

#include "fvm.h"

char code[4096];
char stack[4096];
int code_cursor = 0;
struct fvm vm;

static void reset() {
  vm.pc = (int64_t)code;
  vm.sp = (int64_t)stack + 4095;
  vm.bp = (int64_t)stack + 4095;
  code_cursor = 0;
}

static void emit_op(enum fvm_op op) {
  code[code_cursor] = (char)op;
  code_cursor++;
}
static void emit_num(int64_t x) {
  *(int64_t*)(code + code_cursor) = x;
  code_cursor += 8;
}

void test_imm() {
  printf("[TEST] imm\n");

  reset();

  emit_op(FVM_OP_IMM);
  emit_num(3);
  emit_op(FVM_OP_EXIT);

  assert(fvm_execute(&vm) == 3);

  printf("[PASS] imm\n");
}

int main()
{
  test_imm(); 
  return 0;
}
