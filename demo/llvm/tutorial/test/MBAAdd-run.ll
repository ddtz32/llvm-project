; RUN: %clang++ -S -emit-llvm %S/inputs/MBAAdd.cpp -O2 -o - \
; RUN:   | opt -load-pass-plugin=%lib/libMBAAdd%ext -passes="mba-add" -S -o %t.ll
; RUN: %clang++ %t.ll -o %t.bin

; RUN: %t.bin 1 10 -10 -1
; RUN: not %t.bin 1 2 3 -7
