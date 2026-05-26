// RUN: %clang_cc1 -triple h2blb -Wall -Werror -verify %s

// REQUIRES: h2blb-registered-target

int test(short a) {
  // expected-error@+1 {{too few arguments to function call, expected 2, have 1}}
  return __builtin_h2blb_widening_smul(a);
}
