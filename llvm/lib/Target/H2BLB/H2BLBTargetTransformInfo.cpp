#include "H2BLBTargetTransformInfo.h"
#include "llvm/IR/IntrinsicsH2BLB.h"

using namespace llvm;
using namespace llvm::Intrinsic;

#define DEBUG_TYPE "h2blbtti"

unsigned H2BLBTTIImpl::getLoadVectorFactor(unsigned VF, unsigned LoadSize,
                                           unsigned ChainSizeInBytes,
                                           VectorType *VecTy) const {
  unsigned ElemSize = VecTy->getScalarSizeInBits();
  if (ElemSize != 16)
    return 0;

  return std::min(VF, 2u);
}

InstructionCost H2BLBTTIImpl::getIntrinsicInstrCost(
    const IntrinsicCostAttributes &ICA,
    TargetTransformInfo::TargetCostKind CostKind) const {
  switch (ICA.getID()) {
  default:
    return BaseT::getIntrinsicInstrCost(ICA, CostKind);
  case H2BLBIntrinsics::h2blb_widening_smul:
    return CostKind == TargetTransformInfo::TCK_Latency
               ? TargetTransformInfo::TCC_Expensive
               : TargetTransformInfo::TCC_Basic;
  case H2BLBIntrinsics::h2blb_widening_umul:
    return TargetTransformInfo::TCC_Basic;
  }
}
