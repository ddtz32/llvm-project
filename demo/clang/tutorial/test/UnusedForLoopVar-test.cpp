// RUN: %clang -cc1 -load %lib/libClangTutorial%ext -plugin uflv -verify %s

#define GENERATE_UNUSED_REGULAR_FOR_LOOP_VAR(size, inc) \
  do {\
    for (int x = 0; x < size; x++)\
      inc++;\
  }\
  while (0)

#define GENERATE_UNUSED_RANGE_FOR_LOOP_VAR(arr, inc)                           \
  do {                                                                         \
    for (auto x : arr)                                                         \
      inc++;                                                                   \
  } while (0)

int test() {
  int a = 10;

  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable 'i' not used}}
  for (int i = 0; i < 20; i++) {
    a++;
  }

  // expected-warning@+3 {{(AST Matcher) regular for-loop variable 'i' not used}}
  // expected-warning@+2 {{(Recursive AST Visitor) regular for-loop variable 'i' not used}}
  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable 'j' not used}}
  for (int i = 0, j = 0; j < 20; j++)
    a--;

  int arr[] = {1, 2, 3, 4, 5};
  // expected-warning@+1 {{(AST Matcher) range for-loop variable 'val' not used}}
  for (int val : arr) {
    a++;
  }

  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable 'x' not used}}
  GENERATE_UNUSED_REGULAR_FOR_LOOP_VAR(10, a);

  // expected-warning@+1 {{(AST Matcher) range for-loop variable 'x' not used}}
  GENERATE_UNUSED_RANGE_FOR_LOOP_VAR(arr, a);

  return a;
}
