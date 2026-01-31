class ToyReg<string name, list<string> altNames = []>
    : Register<name, altNames> {
  let Namespace = "Toy";
}

let FallbackRegAltNameIndex = NoRegAltName in
  def ABIRegAltName : RegAltNameIndex;

let RegAltNameIndices = [ABIRegAltName] in {
  def X0  : ToyReg<"x0",  ["zero"]>;
  def X1  : ToyReg<"x1",  ["ra"]>;
  def X2  : ToyReg<"x2",  ["sp"]>;
  def X3  : ToyReg<"x3",  ["gp"]>;
  def X4  : ToyReg<"x4",  ["tp"]>;
  def X5  : ToyReg<"x5",  ["t0"]>;
  def X6  : ToyReg<"x6",  ["t1"]>;
  def X7  : ToyReg<"x7",  ["t2"]>;
  def X8  : ToyReg<"x8",  ["s0", "fp"]>;
  def X9  : ToyReg<"x9",  ["s1"]>;
  def X10 : ToyReg<"x10", ["a0"]>;
  def X11 : ToyReg<"x11", ["a1"]>;
  def X12 : ToyReg<"x12", ["a2"]>;
  def X13 : ToyReg<"x13", ["a3"]>;
  def X14 : ToyReg<"x14", ["a4"]>;
  def X15 : ToyReg<"x15", ["a5"]>;
  def X16 : ToyReg<"x16", ["a6"]>;
  def X17 : ToyReg<"x17", ["a7"]>;
  def X18 : ToyReg<"x18", ["s2"]>;
  def X19 : ToyReg<"x19", ["s3"]>;
  def X20 : ToyReg<"x20", ["s4"]>;
  def X21 : ToyReg<"x21", ["s5"]>;
  def X22 : ToyReg<"x22", ["s6"]>;
  def X23 : ToyReg<"x23", ["s7"]>;
  def X24 : ToyReg<"x24", ["s8"]>;
  def X25 : ToyReg<"x25", ["s9"]>;
  def X26 : ToyReg<"x26", ["s10"]>;
  def X27 : ToyReg<"x27", ["s11"]>;
  def X28 : ToyReg<"x28", ["t3"]>;
  def X29 : ToyReg<"x29", ["t4"]>;
  def X30 : ToyReg<"x30", ["t5"]>;
  def X31 : ToyReg<"x31", ["t6"]>;
}

def GPR : RegisterClass<"Toy", [i32], 32, (add (sequence "X%u", 0, 31))>;
