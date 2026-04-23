; RUN: opt -load-pass-plugin %lib/libOpcodeCounter%ext \
; RUN:   -passes="default<O1>" -disable-output %s \
; RUN:   2>&1 | FileCheck %s

; RUN: opt -load-pass-plugin %lib/libOpcodeCounter%ext \
; RUN:   -passes="default<O2>" -disable-output %s \
; RUN:   2>&1 | FileCheck %s

; RUN: opt -load-pass-plugin %lib/libOpcodeCounter%ext \
; RUN:   -passes="default<O3>" -disable-output %s \
; RUN:   2>&1 | FileCheck %s

; RUN: opt -load-pass-plugin %lib/libOpcodeCounter%ext \
; RUN:   -passes="default<Os>" -disable-output %s \
; RUN:   2>&1 | FileCheck %s

; CHECK-LBBEL: foo
; CHECK:       ret 1
define void @foo() {
  ret void
}
