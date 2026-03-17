//===----------------------------------------------------------------------===//
// Asm Operand
//===----------------------------------------------------------------------===//
class ImmAsmOperand<string prefix, int width, string suffix = "">
    : AsmOperandClass {
  let Name = !strconcat(prefix, "Imm", !cast<string>(width), suffix);
  let PredicateMethod = !strconcat("is", prefix, "Imm", suffix,
                                  "<", !cast<string>(width), ">");
  let RenderMethod = "addImmOperands";
  // ParserMethod default to parseOperand
}

def FenceArgAsmOperand : AsmOperandClass {
  let Name = "FenceArg";
  // PredicateMethod default to isFenceArg
  let RenderMethod = "addFenceArgOperands";
  let ParserMethod = "parseFenceArg";
}

def InsnDirectiveOpcodeAsmOperand : ImmAsmOperand<"U", 7> {
  let Name = "InsnDirectiveOpcode";
  let ParserMethod = "parseInsnDirectiveOpcode";
}

def AUIPCAsmOperand : ImmAsmOperand<"U", 20, "AUIPC"> {
  let PredicateMethod = "isUImm20AUIPC";
}

//===----------------------------------------------------------------------===//
// Operand
//===----------------------------------------------------------------------===//
class ToyOperand : Operand<i32>;

class ImmOperand<string prefix, int width, string suffix = ""> : ToyOperand {
  let ParserMatchClass = ImmAsmOperand<prefix, width, suffix>;
}

class SImmOperand<int width, string suffix = "">
    : ImmOperand<"S", width, suffix> {
  let DecoderMethod = "decodeSImmOperand<" # width # ">";
}
class UImmOperand<int width, string suffix = "">
    : ImmOperand<"U", width, suffix>;
class SImmOperandLsb0<int width> : SImmOperand<width, "Lsb0"> {
  let EncoderMethod = "getImmOpValueAsrN<1>";
  let DecoderMethod = "decodeSImmOperandAndLslN<" # width # ", 1>";

  // OPERAND_PCREL add an extra operand Address
  let OperandType = "OPERAND_PCREL";
  let PrintMethod = "printBranchOperand";
}

def simm12       : SImmOperand<12>;
def simm13_lsb0  : SImmOperandLsb0<13>;
def simm21_lsb0  : SImmOperandLsb0<21>;

def uimm3        : UImmOperand<3>;
def uimm5        : UImmOperand<5>;
def uimm7        : UImmOperand<7>;
def uimm7_opcode : UImmOperand<7> {
  let ParserMatchClass = InsnDirectiveOpcodeAsmOperand;
}
def uimm20       : UImmOperand<20>;
def uimm20_auipc : UImmOperand<20> {
  let ParserMatchClass = AUIPCAsmOperand;
}
def uimm32       : UImmOperand<32>;

def fencearg     : ToyOperand {
  let ParserMatchClass = FenceArgAsmOperand;
}

include "ToyInstrFormats.td"

//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//
class BranchCC_rri<bits<3> funct3, string opcodestr>
    : ToyFormatB<funct3, OPC_BRANCH, (outs),
                 (ins GPR:$rs1, GPR:$rs2, simm13_lsb0:$imm), opcodestr,
                 "$rs1, $rs2, $imm"> {
  // I copied this from the RISCV backend, but I don't know what it's used for yet
  let isBranch       = 1;
  // let isTerminator   = 1;
  let hasSideEffects = 0;
  let mayLoad        = 0;
  let mayStore       = 0;
}

let hasSideEffects = 0, mayLoad = 1, mayStore = 0 in
class Load_ri<bits<3> funct3, string opcodestr>
    : ToyFormatI<funct3, OPC_LOAD, (outs GPR:$rd), (ins GPR:$rs1, simm12:$imm),
                 opcodestr, "$rd, $imm($rs1)">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
class Store_rri<bits<3> funct3, string opcodestr>
    : ToyFormatS<funct3, OPC_STORE, (outs),
                 (ins GPR:$rs2, GPR:$rs1, simm12:$imm), opcodestr,
                 "$rs2, $imm($rs1)">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  class ALU_ri<bits<3> funct3, string opcodestr>
      : ToyFormatI<funct3, OPC_OP_IMM, (outs GPR:$rd),
                   (ins GPR:$rs1, simm12:$imm), opcodestr, "$rd, $rs1, $imm">;

  class Shift_ri<bits<7> imm, bits<3> funct3, string opcodestr>
      : ToyFormatIShift<imm, funct3, OPC_OP_IMM, (outs GPR:$rd),
                        (ins GPR:$rs1, uimm5:$shamt), opcodestr,
                        "$rd, $rs1, $shamt">;

  class ALU_rr<bits<7> funct7, bits<3> funct3, string opcodestr>
      : ToyFormatR<funct7, funct3, OPC_OP, (outs GPR:$rd),
                   (ins GPR:$rs1, GPR:$rs2), opcodestr, "$rd, $rs1, $rs2">;
} // hasSideEffects = 0, mayLoad = 0, mayStore = 0
//===----------------------------------------------------------------------===//
// RV32I/RV64I 
//===----------------------------------------------------------------------===//
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def LUI   : ToyFormatU<OPC_LUI, (outs GPR:$rd), (ins uimm20:$imm),"lui",
                         "$rd, $imm">;

  def AUIPC : ToyFormatU<OPC_AUPIC, (outs GPR:$rd), (ins uimm20_auipc:$imm),
                        "auipc", "$rd, $imm">;

  def JAL   : ToyFormatJ<OPC_JAL, (outs GPR:$rd), (ins simm21_lsb0:$imm), "jal",
                         "$rd, $imm">;

  def JALR  : ToyFormatI<0b000, OPC_JALR, (outs GPR:$rd),
                         (ins GPR:$rs1, simm12:$imm), "jal", "$rd, $imm($rs1)">;
} // let hasSideEffects = 0, mayLoad = 0, mayStore = 0

def BEQ  : BranchCC_rri<0b000, "beq">;
def BNE  : BranchCC_rri<0b001, "bne">;
def BLT  : BranchCC_rri<0b100, "blt">;
def BGE  : BranchCC_rri<0b101, "bge">;
def BLTU : BranchCC_rri<0b110, "bltu">;
def BGEU : BranchCC_rri<0b111, "bgeu">;

def LB  : Load_ri<0b000, "lb">;
def LH  : Load_ri<0b001, "lh">;
def LW  : Load_ri<0b010, "lw">;
def LBU : Load_ri<0b100, "lbu">;
def LHU : Load_ri<0b101, "lhu">;

def SB : Store_rri<0b000, "sb">;
def SH : Store_rri<0b001, "sh">;
def SW : Store_rri<0b010, "sw">;

def ADDI  : ALU_ri<0b000, "addi">;
def SLTI  : ALU_ri<0b010, "slti">;
def SLTIU : ALU_ri<0b011, "sltiu">;
def XORI  : ALU_ri<0b100, "xori">;
def ORI   : ALU_ri<0b110, "ori">;
def ANDI  : ALU_ri<0b111, "andi">;

def SLLI  : Shift_ri<0b0000000, 0b001, "slli">;
def SRLI  : Shift_ri<0b0000000, 0b101, "srli">;
def SRAI  : Shift_ri<0b0100000, 0b101, "srai">;

def ADD  : ALU_rr<0b0000000, 0b000, "add">;
def SUB  : ALU_rr<0b0100000, 0b000, "sub">;
def SLL  : ALU_rr<0b0000000, 0b001, "sll">;
def SLT  : ALU_rr<0b0000000, 0b010, "slt">;
def SLTU : ALU_rr<0b0000000, 0b011, "sltu">;
def XOR  : ALU_rr<0b0000000, 0b100, "xor">;
def SRL  : ALU_rr<0b0000000, 0b101, "srl">;
def SRA  : ALU_rr<0b0100000, 0b101, "sra">;
def OR   : ALU_rr<0b0000000, 0b110, "or">;
def AND  : ALU_rr<0b0000000, 0b111, "and">;


let hasSideEffects = 1, mayLoad = 0, mayStore = 0 in {
  def FENCE : ToyFormatI<0b000, OPC_MISC_MEM, (outs),
                         (ins fencearg:$pred, fencearg:$succ), "fence",
                         "$pred, $succ"> {
    bits<4> pred;
    bits<4> succ;

    let rs1 = 0;
    let rd  = 0;
    let imm = {0b0000, pred, succ};
  }

  def FENCE_TSO : ToyFormatI<0b000, OPC_MISC_MEM, (outs), (ins), "fece.tso", ""> {
    let rs1 = 0;
    let rd  = 0;
    let imm = {0b1000, 0b0011, 0b0011};
  }
} // hasSideEffects = 1, mayLoad = 0, mayStore = 0

// TODO: ECALL, EBREAK
// ---------------------------------- Pseudo ----------------------------------
include "ToyInstrPseudo.td"
include "ToyInstrDirective.td"
// ---------------------------------- Pseudo ----------------------------------
