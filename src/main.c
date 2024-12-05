#include <stdio.h>
#include <stdlib.h>

#include "fvm.h"

static void printnum(struct fvm *vm) {
  int64_t n = fvm_pop(vm);
  printf("%lld\n", n);
}

int main(int argc, char **argv) {
  struct fvm vm;
  if (argc < 2) {
    fprintf(stderr, "Usage: fvm <program-file>\n");
    return -1;
  }
  vm.syscall_table = malloc(256 * sizeof(fvm_syscall_fn_t));
  vm.syscall_table[1] = &printnum;
  FILE* fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    return -1;
  }
  fseek(fp, 0, SEEK_END);
  int64_t flen = ftell(fp);
  char *code = malloc(flen);
  fseek(fp, 0, SEEK_SET);
  int n = fread(code, 1, flen, fp);
  if (n != flen) {
    fprintf(stderr, "Failed to read from file.\n");
    exit(-1);
  }
  fvm_init(&vm, code, 2*1024*1024);
  fvm_execute(&vm);
  return vm.rv;
}
