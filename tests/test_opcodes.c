#include <assert.h>
#include <stdint.h>
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
  vm.rv = 0;
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

#define OP(op) emit_op(FVM_OP_##op)
#define IMM(num)                                                               \
  emit_op(FVM_OP_IMM);                                                         \
  emit_num(num);
#define EXPECT(n) assert(fvm_execute(&vm) == (n));
#define BEGIN(name) void test_##name () { printf("[TEST] " #name  "\n");
#define END(name) printf("[PASS] " #name "\n"); }


BEGIN(imm)
reset();
IMM(3);
OP(SRV);OP(EXIT);
EXPECT(3);
END(imm)


BEGIN(sp)
reset();
int64_t expected = vm.sp;
OP(SP);
OP(SRV);OP(EXIT);
EXPECT(expected);
END(sp)


BEGIN(ssp)
int64_t expected = vm.sp + 4;
//===
reset();
OP(SP);
IMM(4);
OP(ADD);
OP(SSP);
OP(SP);
OP(SRV);OP(EXIT);
EXPECT(expected);
END(ssp)
//===
BEGIN(ld)
int64_t e1 = 0xfefefefefefefefe;
int32_t e2 = 0xfdfdfdfd;
int16_t e3 = 0xfcfc;
int8_t e4 = 0xfb;
//===
reset();
IMM((int64_t)(&e1));
OP(LD);
OP(SRV);OP(EXIT);
EXPECT(e1);
//===
reset();
IMM((int64_t)(&e2));
OP(LD32);
OP(SRV);OP(EXIT);
EXPECT(e2);
//===
reset();
IMM((int64_t)(&e3));
OP(LD16);
OP(SRV);OP(EXIT);
EXPECT(e3);
//===
reset();
IMM((int64_t)(&e4));
OP(LD8);
OP(SRV);OP(EXIT);
EXPECT(e4);
END(ld)


BEGIN(st)
int64_t loc = 0;
int32_t loc32 = 0;
int16_t loc16 = 0;
int8_t loc8 = 0;
int64_t e1 = 0xfefefefefefefefe;
int32_t e2 = 0xfdfdfdfd;
int16_t e3 = 0xfcfc;
int8_t e4 = 0xfb;
//===
reset();
IMM(42);
IMM(e1);
IMM((int64_t)(&loc));
OP(ST);
OP(SRV);OP(EXIT);
assert(fvm_execute(&vm) == 42);
assert(loc == e1);
//===
reset();
IMM(42);
IMM(e2);
IMM((int64_t)(&loc32));
OP(ST32);
OP(SRV);OP(EXIT);
assert(fvm_execute(&vm) == 42);
assert(loc32 == e2);
//===
reset();
IMM(42);
IMM(e3);
IMM((int64_t)(&loc16));
OP(ST16);
OP(SRV);OP(EXIT);
assert(fvm_execute(&vm) == 42);
assert(loc16 == e3);
//===
reset();
IMM(42);
IMM(e4);
IMM((int64_t)(&loc8));
OP(ST8);
OP(SRV);OP(EXIT);
assert(fvm_execute(&vm) == 42);
assert(loc8 == e4);
END(st)


BEGIN(dup)
reset();
IMM(42);
OP(DUP);
OP(SRV);OP(EXIT);
EXPECT(42);
//===
reset();
IMM(42);
OP(DUP);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(42);
//===
reset();
IMM(3);
IMM(42);
OP(DUP);
OP(POP);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(3);
END(dup)

BEGIN(swap)
reset();
IMM(1);
IMM(2);
OP(SWAP);
OP(SRV);OP(EXIT);
EXPECT(1);
//===
reset();
IMM(1);
IMM(2);
OP(SWAP);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(2);
END(swap)

BEGIN(over)
reset();
IMM(1);
IMM(2);
OP(OVER);
OP(SRV);OP(EXIT);
EXPECT(1);
//===
reset();
IMM(1);
IMM(2);
OP(OVER);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(2);
//===
reset();
IMM(1);
IMM(2);
OP(OVER);
OP(POP);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(1);
END(over)

BEGIN(add)
reset();
IMM(1);
IMM(2);
OP(ADD);
OP(SRV);OP(EXIT);
EXPECT(3);
//===
reset();
IMM(42);
IMM(1);
IMM(2);
OP(ADD);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(42);
END(add)

BEGIN(sub)
reset();
IMM(1);
IMM(2);
OP(SUB);
OP(SRV);OP(EXIT);
EXPECT(-1);
//===
reset();
IMM(42);
IMM(1);
IMM(2);
OP(ADD);
OP(POP);
OP(SRV);OP(EXIT);
EXPECT(42);
END(sub)

BEGIN(jmp)
reset();
IMM((int64_t)code + 14);
OP(JMP);
OP(SRV);OP(EXIT);
OP(SRV);OP(EXIT);
IMM(42);
OP(SRV);OP(EXIT);
EXPECT(42);
END(jmp)

BEGIN(jz)
reset();
IMM(0);
IMM((int64_t)code + 32);
OP(JZ);
IMM(21);
OP(SRV);OP(EXIT);
OP(SRV);OP(EXIT);
IMM(42);
OP(SRV);OP(EXIT);
EXPECT(42);
//===
reset();
IMM(-1);
IMM((int64_t)code + 30);
OP(JZ);
IMM(21);
OP(SRV);OP(EXIT);
OP(SRV);OP(EXIT);
IMM(42);
OP(SRV);OP(EXIT);
EXPECT(21);
END(jz)

BEGIN(jnz)
reset();
IMM(0);
IMM((int64_t)code + 32);
OP(JNZ);
IMM(21);
OP(SRV);OP(EXIT);
OP(SRV);OP(EXIT);
IMM(42);
OP(SRV);OP(EXIT);
EXPECT(21);
//===
reset();
IMM(-1);
IMM((int64_t)code + 32);
OP(JNZ);
IMM(21);
OP(SRV);OP(EXIT);
OP(SRV);OP(EXIT);
IMM(42);
OP(SRV);OP(EXIT);
EXPECT(42);
END(jnz)

int main()
{
  test_imm();
  test_sp();
  test_ssp();
  test_ld();
  test_st();
  test_dup();
  test_swap();
  test_over();
  test_add();
  test_sub();
  test_jmp();
  test_jz();
  test_jnz();
  return 0;
}
