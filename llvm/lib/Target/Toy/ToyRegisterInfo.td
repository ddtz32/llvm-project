class ToyReg<string name, list<string> altNames = []>
    : Register<name, altNames> {
  let Namespace = "Toy";
}

let FallbackRegAltNameIndex = NoRegAltName in
  def ABIRegAltName : RegAltNameIndex;

let RegAltNameIndices = [ABIRegAltName] in {
  def X0  : ToyReg<"X0",  ["zero"]>;
  def X1  : ToyReg<"X1",  ["ra"]>;
  def X2  : ToyReg<"X2",  ["sp"]>;
  def X3  : ToyReg<"X3",  ["gp"]>;
  def X4  : ToyReg<"X4",  ["tp"]>;
  def X5  : ToyReg<"X5",  ["t0"]>;
  def X6  : ToyReg<"X6",  ["t1"]>;
  def X7  : ToyReg<"X7",  ["t2"]>;
  def X8  : ToyReg<"X8",  ["s0", "fp"]>;
  def X9  : ToyReg<"X9",  ["s1"]>;
  def X10 : ToyReg<"X10", ["a0"]>;
  def X11 : ToyReg<"X11", ["a1"]>;
  def X12 : ToyReg<"X12", ["a2"]>;
  def X13 : ToyReg<"X13", ["a3"]>;
  def X14 : ToyReg<"X14", ["a4"]>;
  def X15 : ToyReg<"X15", ["a5"]>;
  def X16 : ToyReg<"X16", ["a6"]>;
  def X17 : ToyReg<"X17", ["a7"]>;
  def X18 : ToyReg<"X18", ["s2"]>;
  def X19 : ToyReg<"X19", ["s3"]>;
  def X20 : ToyReg<"X20", ["s4"]>;
  def X21 : ToyReg<"X21", ["s5"]>;
  def X22 : ToyReg<"X22", ["s6"]>;
  def X23 : ToyReg<"X23", ["s7"]>;
  def X24 : ToyReg<"X24", ["s8"]>;
  def X25 : ToyReg<"X25", ["s9"]>;
  def X26 : ToyReg<"X26", ["s10"]>;
  def X27 : ToyReg<"X27", ["s11"]>;
  def X28 : ToyReg<"X28", ["t3"]>;
  def X29 : ToyReg<"X29", ["t4"]>;
  def X30 : ToyReg<"X30", ["t5"]>;
  def X31 : ToyReg<"X31", ["t6"]>;
}

def GPR : RegisterClass<"Toy", [i32], 32, (add (sequence "X%u", 0, 31))>;
