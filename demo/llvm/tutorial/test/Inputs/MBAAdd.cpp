#include <cstdlib>

int main(int argc, char **argv) {
  int8_t a = atoi(argv[1]);
  int8_t b = atoi(argv[2]);
  int8_t c = atoi(argv[3]);
  int8_t d = atoi(argv[4]);

  int e = a + b;
  int f = c + d;
  return e + f;
}
