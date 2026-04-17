// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin print-something %s \
// RUN:   2>&1 | FileCheck %s

// CHECK: count: 3, file: {{.*}}/demo/clang/tutorial/test/PrintSomething-basic.cpp

class A {
  int a;
};

struct B {
  float b;
};

union C {
  A a;
  B b;
};
