#include "H2BLBTargetTransformInfo.h"

using namespace llvm;

#define DEBUG_TYPE "h2blbtti"

unsigned H2BLBTTIImpl::getLoadVectorFactor(unsigned VF, unsigned LoadSize,
                                           unsigned ChainSizeInBytes,
                                           VectorType *VecTy) const {
  unsigned ElemSize = VecTy->getScalarSizeInBits();
  if (ElemSize != 16)
    return 0;

  return std::min(VF, 2u);
}
