class ToyOpcode<string name, bits<7> opcode> {
  string Name = name;
  bits<7> Opcode = opcode;
}

// riscv-unprivileged.pdf ch36 Table 72
def OPC_LOAD      : ToyOpcode<"LOAD",      0b0000011>;
def OPC_LOAD_FP   : ToyOpcode<"LOAD_FP",   0b0000111>;
def OPC_CUSTOM_0  : ToyOpcode<"CUSTOM_0",  0b0001011>;
def OPC_MISC_MEM  : ToyOpcode<"MISC_MEM",  0b0001111>;
def OPC_OP_IMM    : ToyOpcode<"OP_IMM",    0b0010011>;
def OPC_AUPIC     : ToyOpcode<"AUIPC",     0b0010111>;
def OPC_OP_IMM_32 : ToyOpcode<"OP_IMM_32", 0b0011011>;
def OPC_STORE     : ToyOpcode<"STORE",     0b0100011>;
def OPC_STORE_FP  : ToyOpcode<"STORE_FP",  0b0100111>;
def OPC_CUSTOM_1  : ToyOpcode<"CUSTOM_1",  0b0101011>;
def OPC_AMO       : ToyOpcode<"AMO",       0b0101111>;
def OPC_OP        : ToyOpcode<"OP",        0b0110011>;
def OPC_LUI       : ToyOpcode<"LUI",       0b0110111>;
def OPC_OP_32     : ToyOpcode<"OP_32",     0b0111011>;
def OPC_MADD      : ToyOpcode<"MADD",      0b1000011>;
def OPC_MSUB      : ToyOpcode<"MSUB",      0b1000111>;
def OPC_NMSUB     : ToyOpcode<"NMSUB",     0b1001011>;
def OPC_NMADD     : ToyOpcode<"NMADD",     0b1001111>;
def OPC_OP_FP     : ToyOpcode<"OP_FP",     0b1010011>;
def OPC_OP_V      : ToyOpcode<"OP_V",      0b1010111>;
def OPC_CUSTOM_2  : ToyOpcode<"CUSTOM_2",  0b1011011>;
def OPC_BRANCH    : ToyOpcode<"BRANCH",    0b1100011>;
def OPC_JALR      : ToyOpcode<"JALR",      0b1100111>;
def OPC_JAL       : ToyOpcode<"JAL",       0b1101111>;
def OPC_SYSTEM    : ToyOpcode<"SYSTEM",    0b1110011>;
def OPC_OP_VE     : ToyOpcode<"OP_VE",     0b1110111>;
def OPC_CUSTOM_3  : ToyOpcode<"CUSTOM_3",  0b1111011>;

class ToyInst<dag outs, dag ins, bits<7> opcode, string opcodestr,
              string argstr> : Instruction {
  field bits<32> Inst;
  let Inst{6-0} = opcode;

  let Namespace = "Toy";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = opcodestr # !if(!empty(argstr), "", "\t" # argstr);
}

class ToyFormatR<bits<7> opcode, bits<3> funct3, bits<7> funct7, string opcodestr>
    : ToyInst<(outs GPR:$rd), (ins GPR:$rs1, GPR:$rs2), opcode, opcodestr,
              "$rd, $rs1, $rs2"> {
  bits<5> rd;
  bits<5> rs1;
  bits<5> rs2;

  let Inst{11-7}  = rd;
  let Inst{14-12} = funct3;
  let Inst{19-15} = rs1;
  let Inst{24-20} = rs2;
  let Inst{31-25} = funct7;
}

class ToyFormatIBase<dag outs, dag ins, bits<7> opcode, bits<3> funct3,
                     string opcodestr, string argstr>
    : ToyInst<outs, ins, opcode, opcodestr, argstr> {
  bits<5> rd;
  bits<5> rs1;

  let Inst{11-7}  = rd;
  let Inst{14-12} = funct3;
  let Inst{19-15} = rs1;
}

class ToyFormatI<bits<7> opcode, bits<3> funct3, string opcodestr>
    : ToyFormatIBase<(outs GPR:$rd), (ins GPR:$rs1, simm12:$imm), opcode,
                     funct3, opcodestr, "$rd, $rs1, $imm"> {
  bits<12> imm;

  let Inst{31-20} = imm;
}

class ToyFormatIShift<bits<7> opcode, bits<3> funct3, bits<7> imm12_hi, string opcodestr>
    : ToyFormatIBase<(outs GPR:$rd), (ins GPR:$rs1, uimm5:$shamt), opcode,
                     funct3, opcodestr, "$rd, $rs1, $shamt"> {
  bits<5> shamt;

  let Inst{24-20} = shamt;
  let Inst{31-25} = imm12_hi;
}

class ToyFormatS<bits<7> opcode, bits<3> funct3, string opcodestr>
    : ToyInst<(outs), (ins GPR:$rs1, GPR:$rs2, simm12:$imm), opcode,
              opcodestr, "$rs1, $rs2, $imm"> {
  bits<5> rs1;
  bits<5> rs2;
  bits<12> imm;

  let Inst{11-7}  = imm{4-0};
  let Inst{14-12} = funct3;
  let Inst{19-15} = rs1;
  let Inst{24-20} = rs2;
  let Inst{31-25} = imm{11-5};
}

class ToyFormatB<bits<7> opcode, bits<3> funct3, string opcodestr>
    : ToyInst<(outs), (ins GPR:$rs1, GPR:$rs2, simm13_lsb0:$imm), opcode,
              opcodestr, "$rs1, $rs2, $imm"> {
  bits<5> rs1;
  bits<5> rs2;
  bits<13> imm;

  let Inst{7}     = imm{11};
  let Inst{11-8}  = imm{4-1};
  let Inst{14-12} = funct3;
  let Inst{19-15} = rs1;
  let Inst{24-20} = rs2;
  let Inst{30-25} = imm{10-5};
  let Inst{31}    = imm{12};
}

class ToyFormatU<bits<7> opcode, string opcodestr>
    : ToyInst<(outs GPR:$rd), (ins uimm20:$imm), opcode,
              opcodestr, "$rd, $imm"> {
  bits<5> rd;
  bits<20> imm;

  let Inst{11-7}  = rd;
  let Inst{31-12} = imm;
}

class ToyFormatJ<bits<7> opcode, string opcodestr>
    : ToyInst<(outs GPR:$rd), (ins simm21_lsb0:$imm), opcode,
              opcodestr, "$rd, $imm"> {
  bits<5> rd;
  bits<21> imm;

  let Inst{11-7}  = rd;
  let Inst{19-12} = imm{19-12};
  let Inst{20}    = imm{11};
  let Inst{30-21} = imm{10-1};
  let Inst{31}    = imm{20};
}
