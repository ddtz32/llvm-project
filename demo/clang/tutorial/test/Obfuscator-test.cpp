// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin obfuscator %s \
// RUN:   2>&1 | FileCheck %s

// CHECK-LABEL: int testInt(int a, int b) {
// CHECK-NEXT:  int c = (123 ^ 321) + 2 * (123 & 321);
// CHECK-NEXT:  c = (c ^ a) + 2 * (c & a);
// CHECK-NEXT:  c = (c + ~b) + 1;
// CHECK-NEXT:  c = (c ^ 1) + 2 * (c & 1);
// CHECK-NEXT:  c = (c + ~2) + 1;
// CHECK-NEXT:  return c;
int testInt(int a, int b) {
  int c = 123 + 321;
  c = c + a;
  c = c - b;
  c = c + 1;
  c = c - 2;
  return c;
}

// CHECK-LABEL: float testFloat(float a, float b) {
// CHECK-NEXT:  float c = a + b;
// CHECK-NEXT:  return c;
float testFloat(float a, float b) {
  float c = a + b;
  return c;
}
