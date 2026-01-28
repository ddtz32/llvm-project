#pragma once

namespace llvm {

class Target;

Target &getTheToy32Target();
Target &getTheToy64Target();

} // namespace llvm
