#pragma once

#include "H2BLBISelLowering.h"
#include "H2BLBSubtarget.h"
#include "H2BLBTargetMachine.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {

class H2BLBTTIImpl final : public BasicTTIImplBase<H2BLBTTIImpl> {
  using BaseT = BasicTTIImplBase<H2BLBTTIImpl>;
  friend BaseT;

  const H2BLBSubtarget *ST;
  const H2BLBTargetLowering *TLI;

  const H2BLBSubtarget *getST() const { return ST; }
  const H2BLBTargetLowering *getTLI() const { return TLI; }

public:
  explicit H2BLBTTIImpl(const H2BLBTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  unsigned getLoadVectorFactor(unsigned VF, unsigned LoadSize,
                               unsigned ChainSizeInBytes,
                               VectorType *VecTy) const override;

  InstructionCost getIntrinsicInstrCost(
      const IntrinsicCostAttributes &ICA,
      TargetTransformInfo::TargetCostKind CostKind) const override;
};

} // namespace llvm
