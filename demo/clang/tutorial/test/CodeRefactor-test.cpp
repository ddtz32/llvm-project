// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin code-refactor \
// RUN:   -plugin-arg-code-refactor -class-name -plugin-arg-code-refactor Base \
// RUN:   -plugin-arg-code-refactor -old-name -plugin-arg-code-refactor run \
// RUN:   -plugin-arg-code-refactor -new-name -plugin-arg-code-refactor foo %s \
// RUN:   2>&1 | FileCheck %s

// RUN: ../bin/code-refactor -class-name Base -old-name run -new-name foo %s \
// RUN:   2>&1 | FileCheck %s

// CHECK-LABEL: struct Base {
// CHECK:       virtual void foo() {};
struct Base {
  virtual void run() {};
};

// CHECK-LABEL: struct Derived {
// CHECK:       void foo() override {};
struct Derived : public Base {
  void run() override {};
};

// CHECK-LABEL: void test() {
// CHECK:       B1.foo();
// CHECK:       D1.foo();
// CHECK:       B2->foo();
// CHECK:       D2->foo();
// CHECK:       D3->foo();
void test() {
  Base B1;
  Derived D1;

  B1.run();
  D1.run();

  Base *B2 = new Base();
  Base *D2 = new Derived();
  Derived *D3 = new Derived();

  B2->run();
  D2->run();
  D3->run();
}
