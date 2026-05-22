let TargetPrefix = "h2blb" in {
  def int_h2blb_widening_smul :
    Intrinsic<[llvm_i32_ty], [llvm_i16_ty, llvm_i16_ty]>;
  def int_h2blb_widening_umul :
    Intrinsic<[llvm_i32_ty], [llvm_i16_ty, llvm_i16_ty]>;
} // let TargetPrefix = "h2blb"
