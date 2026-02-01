class ToyReg<bits<5> encodeing, string name, list<string> altNames = []>
    : Register<name, altNames> {
  let Namespace = "Toy";
  let HWEncoding{4-0} = encodeing;
}

let FallbackRegAltNameIndex = NoRegAltName in
  def ABIRegAltName : RegAltNameIndex;

let RegAltNameIndices = [ABIRegAltName] in {
  def X0  : ToyReg<0,  "x0",  ["zero"]>;
  def X1  : ToyReg<1,  "x1",  ["ra"]>;
  def X2  : ToyReg<2,  "x2",  ["sp"]>;
  def X3  : ToyReg<3,  "x3",  ["gp"]>;
  def X4  : ToyReg<4,  "x4",  ["tp"]>;
  def X5  : ToyReg<5,  "x5",  ["t0"]>;
  def X6  : ToyReg<6,  "x6",  ["t1"]>;
  def X7  : ToyReg<7,  "x7",  ["t2"]>;
  def X8  : ToyReg<8,  "x8",  ["s0", "fp"]>;
  def X9  : ToyReg<9,  "x9",  ["s1"]>;
  def X10 : ToyReg<10, "x10", ["a0"]>;
  def X11 : ToyReg<11, "x11", ["a1"]>;
  def X12 : ToyReg<12, "x12", ["a2"]>;
  def X13 : ToyReg<13, "x13", ["a3"]>;
  def X14 : ToyReg<14, "x14", ["a4"]>;
  def X15 : ToyReg<15, "x15", ["a5"]>;
  def X16 : ToyReg<16, "x16", ["a6"]>;
  def X17 : ToyReg<17, "x17", ["a7"]>;
  def X18 : ToyReg<18, "x18", ["s2"]>;
  def X19 : ToyReg<19, "x19", ["s3"]>;
  def X20 : ToyReg<20, "x20", ["s4"]>;
  def X21 : ToyReg<21, "x21", ["s5"]>;
  def X22 : ToyReg<22, "x22", ["s6"]>;
  def X23 : ToyReg<23, "x23", ["s7"]>;
  def X24 : ToyReg<24, "x24", ["s8"]>;
  def X25 : ToyReg<25, "x25", ["s9"]>;
  def X26 : ToyReg<26, "x26", ["s10"]>;
  def X27 : ToyReg<27, "x27", ["s11"]>;
  def X28 : ToyReg<28, "x28", ["t3"]>;
  def X29 : ToyReg<29, "x29", ["t4"]>;
  def X30 : ToyReg<30, "x30", ["t5"]>;
  def X31 : ToyReg<31, "x31", ["t6"]>;
}

def GPR : RegisterClass<"Toy", [i32], 32, (add (sequence "X%u", 0, 31))>;
