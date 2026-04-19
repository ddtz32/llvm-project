// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin uflv -verify %s

// expected-no-diagnostics

int test() {
  int a = 10;

  for (int Unused = 0, unuseD = 0; Unused < 20; ++Unused)
    a++;

  int arr[] = {1, 2, 3, 4, 5};
  for (int UNUSED : arr)
    a++;

  return a;
}
