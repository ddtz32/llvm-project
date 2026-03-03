class ToyOpcode<string name, bits<7> val> {
  string  Name  = name;
  bits<7> Value = val;
}

def ToyOpcodesList : GenericTable {
  let FilterClass = "ToyOpcode";
  let Fields = [ "Name", "Value" ];
  let PrimaryKey = [ "Value" ];
  let PrimaryKeyName = "lookupToyOpcodeByValue";
}

def lookupToyOpcodeByName : SearchIndex{
  let Table = ToyOpcodesList;
  let Key = [ "Name" ];
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

// NOTE: Except for the 5-bit immediates used in CSR instructions, immediates
// are always sign-extended
class ToyInst<dag outs, dag ins, string opcodestr, string argstr>
    : Instruction {
  field bits<32> Inst;

  let Namespace      = "Toy";
  let OutOperandList = outs;
  let InOperandList  = ins;
  let AsmString      = opcodestr # !if(!empty(argstr), "", "\t" # argstr);
  let Size           = 4;
}

class ToyFormatR<bits<7> funct7, bits<3> funct3, ToyOpcode opcode, dag outs,
                 dag ins, string opcodestr, string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = funct7;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7}  = rd;
  let Inst{6-0}   = opcode.Value;
}

class ToyFormatIBase<bits<3> funct3, ToyOpcode opcode, dag outs, dag ins,
                     string opcodestr, string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  bits<5> rs1;
  bits<5> rd;

  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7}  = rd;
  let Inst{6-0}   = opcode.Value;
}

class ToyFormatI<bits<3> funct3, ToyOpcode opcode, dag outs, dag ins,
                 string opcodestr, string argstr>
    : ToyFormatIBase<funct3, opcode, outs, ins, opcodestr, argstr> {
  bits<12> imm;

  let Inst{31-20} = imm;
}

class ToyFormatIShift<bits<7> imm, bits<3> funct3, ToyOpcode opcode, dag outs,
                      dag ins, string opcodestr, string argstr>
    : ToyFormatIBase<funct3, opcode, outs, ins, opcodestr, argstr> {
  bits<5> shamt;

  let Inst{31-25} = imm;
  let Inst{24-20} = shamt;
}

class ToyFormatS<bits<3> funct3, ToyOpcode opcode, dag outs, dag ins,
                 string opcodestr, string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  bits<12> imm;
  bits<5> rs2;
  bits<5> rs1;

  let Inst{31-25} = imm{11-5};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7}  = imm{4-0};
  let Inst{6-0}   = opcode.Value;
}

class ToyFormatB<bits<3> funct3, ToyOpcode opcode, dag outs, dag ins,
                 string opcodestr, string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  bits<12> imm;
  bits<5> rs2;
  bits<5> rs1;

  let Inst{31}    = imm{11};
  let Inst{30-25} = imm{9-4};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-8}  = imm{3-0};
  let Inst{7}     = imm{10};
  let Inst{6-0}   = opcode.Value;
}

class ToyFormatU<ToyOpcode opcode, dag outs, dag ins, string opcodestr,
                string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  bits<20> imm;
  bits<5> rd;

  let Inst{31-12} = imm;
  let Inst{11-7}  = rd;
  let Inst{6-0}   = opcode.Value;
}

class ToyFormatJ<ToyOpcode opcode, dag outs, dag ins, string opcodestr,
                string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  bits<20> imm;
  bits<5> rd;

  let Inst{31}    = imm{19};
  let Inst{30-21} = imm{9-0};
  let Inst{20}    = imm{10};
  let Inst{19-12} = imm{18-11};
  let Inst{11-7}  = rd;
  let Inst{6-0}   = opcode.Value;
}
