// RUN: %clang -cc1 -load %lib/libPrintSomething.so -plugin print-something %s \
// RUN:   2>&1 | FileCheck %s

// CHECK: no declarations found

int val;

void foo();
