; RUN: opt -load-pass-plugin %lib/libOpcodeCounter%ext \
; RUN:   -passes="print<opcode-counter>" -disable-output %S/Inputs/Call.ll \
; RUN:   2>&1 | FileCheck %s

; CHECK-LABEL: foo
; CHECK:       ret 1

; CHECK-LABEL: bar
; CHECK:       call 1
; CHECK-NEXT:  ret  1

; CHECK-LABEL: fez
; CHECK:       call 1
; CHECK-NEXT:  ret  1

; CHECK-LABEL: main
; CHECK:       add    1
; CHECK-NEXT:  call   4
; CHECK-NEXT:  ret    1
; CHECK-NEXT:  load   2
; CHECK-NEXT:  br     4
; CHECK-NEXT:  alloca 2
; CHECK-NEXT:  store  4
; CHECK-NEXT:  icmp   1
