; RUN: opt -load-pass-plugin=%lib/libMBASub%ext -passes="mba-sub" -S %s \
; RUN:   | FileCheck %s


define i32 @foo(i32, i32, i32, i32) {
  %5 = sub i32 %1, %0
  %6 = sub i32 %5, %2
  %7 = sub i32 %6, %3
  ret i32 %7
}

; CHECK-LABEL: define i32 @foo(i32 %0, i32 %1, i32 %2, i32 %3)

; CHECK:       [[REG_1:%[0-9]+]] = xor i32 {{%[0-9]+}}, -1
; CHECK-NEXT:  [[REG_2:%[0-9]+]] = add i32 {{%[0-9]+}}, [[REG_1]]
; CHECK-NEXT:  [[REG_3:%[0-9]+]] = add i32 [[REG_2]], 1

; CHECK:       [[REG_4:%[0-9]+]] = xor i32 {{%[0-9]+}}, -1
; CHECK-NEXT:  [[REG_5:%[0-9]+]] = add i32 [[REG_3]], [[REG_4]]
; CHECK-NEXT:  [[REG_6:%[0-9]+]] = add i32 [[REG_5]], 1
;
; CHECK:       [[REG_7:%[0-9]+]] = xor i32 {{%[0-9]+}}, -1
; CHECK-NEXT:  [[REG_8:%[0-9]+]] = add i32 [[REG_6]], [[REG_7]]
; CHECK-NEXT:  [[REG_9:%[0-9]+]] = add i32 [[REG_8]], 1

;CHECK-NOT:    sub
;CHECK-NOT:    xor
;CHECK-NOT:    add
