//===----------------------------------------------------------------------===//
// .insn directives
//===----------------------------------------------------------------------===//
class DirectiveInsn<string format, dag outs, dag ins, string argstr>
    : ToyInst<outs, ins, "", ""> {
  bits<7> opcode;

  let Inst{6-0}      = opcode;

  let AsmString      = !strconcat(".insn ", format, " ", argstr);
  let isCodeGenOnly  = 1;
  let hasSideEffects = 0;
  let mayLoad        = 0;
  let mayStore       = 0;
}

class DirectiveInsnR<dag outs, dag ins, string argstr>
    : DirectiveInsn<"r", outs, ins, argstr> {
  bits<7> funct7;
  bits<5> rs2;
  bits<5> rs1;
  bits<3> funct3;
  bits<5> rd;

  let Inst{31-25} = funct7;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7}  = rd;
}

let hasSideEffects = 1, mayLoad = 1 in
class DirectiveInsnI<dag outs, dag ins, string argstr>
    : DirectiveInsn<"i", outs, ins, argstr> {
  bits<12> imm;
  bits<5> rs1;
  bits<3> funct3;
  bits<5> rd;

  let Inst{31-20} = imm;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7}  = rd;
}

let mayStore = 1 in
class DirectiveInsnS<dag outs, dag ins, string argstr>
    : DirectiveInsn<"s", outs, ins, argstr> {
  bits<12> imm;
  bits<5> rs2;
  bits<5> rs1;
  bits<3> funct3;

  let Inst{31-25} = imm{11-5};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7}  = imm{4-0};
}

class DirectiveInsnB<dag outs, dag ins, string argstr>
    : DirectiveInsn<"b", outs, ins, argstr> {
  bits<12> imm;
  bits<5> rs2;
  bits<5> rs1;
  bits<3> funct3;

  let Inst{31}    = imm{11};
  let Inst{30-25} = imm{9-4};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-8}  = imm{3-0};
  let Inst{7}     = imm{10};
}

class DirectiveInsnU<dag outs, dag ins, string argstr>
    : DirectiveInsn<"u", outs, ins, argstr> {
  bits<20> imm;
  bits<5> rd;

  let Inst{31-12} = imm;
  let Inst{11-7}  = rd;
}

class DirectiveInsnJ<dag outs, dag ins, string argstr>
    : DirectiveInsn<"j", outs, ins, argstr> {
  bits<20> imm;
  bits<5> rd;

  // TODO: Is this correct?
  // let Inst{31}    = imm{19};
  // let Inst{30-21} = imm{9-0};
  // let Inst{20}    = imm{10};
  // let Inst{19-12} = imm{18-11};
  let Inst{31-12} = imm;
  let Inst{11-7}  = rd;
}

def Insn : ToyInst<(outs), (ins uimm32:$value), "", ""> {
  bits<32> value;

  let Inst           = value;

  let AsmString      = ".insn 0x4, $value";
  let isCodeGenOnly  = 1;
  let hasSideEffects = 1;
  let mayLoad        = 1;
  let mayStore       = 1;
}

def InsnR : DirectiveInsnR<(outs GPR:$rd), (ins uimm7_opcode:$opcode,
                                                uimm3:$funct3, uimm7:$funct7,
                                                GPR:$rs1, GPR:$rs2),
                           "$opcode, $funct3, $funct7, $rd, $rs1, $rs2">;
def InsnI : DirectiveInsnI<(outs GPR:$rd), (ins uimm7_opcode:$opcode,
                                                uimm3:$funct3, GPR:$rs1,
                                                simm12:$imm),
                           "$opcode, $funct3, $rd, $rs1, $imm">;
def InsnS : DirectiveInsnS<(outs), (ins uimm7_opcode:$opcode, uimm3:$funct3,
                                        GPR:$rs1, GPR:$rs2, simm12:$imm),
                           "$opcode, $funct3, $rs1, $rs2, $imm">;
def InsnB : DirectiveInsnB<(outs), (ins uimm7_opcode:$opcode, uimm3:$funct3,
                                        GPR:$rs1, GPR:$rs2, simm13_lsb0:$imm),
                           "$opcode, $funct3, $rs1, $rs2, $imm">;
def InsnU : DirectiveInsnU<(outs GPR:$rd), (ins uimm7_opcode:$opcode,
                                                uimm20:$imm),
                           "$opcode, $rd, $imm">;
def InsnJ : DirectiveInsnJ<(outs GPR:$rd), (ins uimm7_opcode:$opcode,
                                                simm21_lsb0:$imm),
                           "$opcode, $rd, $imm">;

def : InstAlias<".insn_r $opcode, $funct3, $funct7, $rd, $rs1, $rs2",
                (InsnR GPR:$rd, uimm7_opcode:$opcode, uimm3:$funct3,
                       uimm7:$funct7, GPR:$rs1, GPR:$rs2)>;
def : InstAlias<".insn_i $opcode, $funct3, $rd, $rs1, $imm",
                (InsnI GPR:$rd, uimm7_opcode:$opcode, uimm3:$funct3, GPR:$rs1,
                       simm12:$imm)>;
def : InstAlias<".insn_s $opcode, $funct3, $rs1, $rs2, $imm",
                (InsnS uimm7_opcode:$opcode, uimm3:$funct3, GPR:$rs1, GPR:$rs2,
                       simm12:$imm)>;
def : InstAlias<".insn_b $opcode, $funct3, $rs1, $rs2, $imm",
                (InsnB uimm7_opcode:$opcode, uimm3:$funct3, GPR:$rs1, GPR:$rs2,
                       simm13_lsb0:$imm)>;
def : InstAlias<".insn_u $opcode, $rd, $imm",
                (InsnU GPR:$rd, uimm7_opcode:$opcode, uimm20:$imm)>;
def : InstAlias<".insn_j $opcode, $rd, $imm",
                (InsnJ GPR:$rd, uimm7_opcode:$opcode, simm21_lsb0:$imm)>;
