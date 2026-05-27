include "clang/Basic/BuiltinsBase.td"

class H2BLBBuiltin<string prototype, string features = ""> : TargetBuiltin {
  let Spellings = ["__builtin_h2blb_" # NAME];
  let Prototype = prototype;
  let Features = features;
}

let Attributes = [NoThrow, Const] in {
def widening_smul : H2BLBBuiltin<"int(short, short)">;
def widening_umul : H2BLBBuiltin<"unsigned int(unsigned short, unsigned short)">;
}
