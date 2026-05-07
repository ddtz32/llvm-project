; RUN: opt -load-pass-plugin=%lib/libMBAAdd%ext -passes="mba-add" -S %s \
; RUN:   | FileCheck %s


define i8 @foo(i8, i8) {
  %3 = add i8 %0, %1
  ret i8 %3
}

define i32 @bar(i32, i32) {
  %3 = add i32 %0, %1
  ret i32 %3
}

; CHECK-LABEL: define i8 @foo(i8 %0, i8 %1)
; CHECK:       [[REG_3:%[0-9]+]] = xor i8 [[REG_1:%[0-9]+]], [[REG_2:%[0-9]+]]
; CHECK-NEXT:  [[REG_4:%[0-9]+]] = and i8 [[REG_1]], [[REG_2]]
; CHECK-NEXT:  [[REG_5:%[0-9]+]] = mul i8 2, [[REG_4]]
; CHECK-NEXT:  [[REG_6:%[0-9]+]] = add i8 [[REG_3]], [[REG_5]]
; CHECK-NEXT:  [[REG_7:%[0-9]+]] = mul i8 [[REG_6]], 39
; CHECK-NEXT:  [[REG_8:%[0-9]+]] = add i8 [[REG_7]], 23
; CHECK-NEXT:  [[REG_9:%[0-9]+]] = mul i8 [[REG_8]], -105
; CHECK-NEXT:  [[REG_10:%[0-9]+]] = add i8 [[REG_9]], 111
; CHECK-NEXT:  ret i8 [[REG_10]]

; CHECK-LABEL: define i32 @bar(i32 %0, i32 %1)
; CHECK:       [[REG_11:%[0-9]+]] = add i32 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-NEXT:  ret i32 [[REG_11]]
