def simm12      : Operand<i32>, ImmLeaf<i32, "return isInt<12>(Imm);">;
def simm13_lsb0 : Operand<i32>, ImmLeaf<i32, "return isShiftedInt<12, 1>(Imm);">;
def simm21_lsb0 : Operand<i32>, ImmLeaf<i32, "return isShiftedInt<21, 1>(Imm);">;

def uimm5       : Operand<i32>, ImmLeaf<i32, "return isUint<5>(Imm);">;
def uimm20      : Operand<i32>, ImmLeaf<i32, "return isUint<20>(Imm);">;

include "ToyInstrFormats.td"

// -------------------------------- RV32I/RV64I -------------------------------
let mayLoad = 0, mayStore = 0, hasSideEffects = 0 in {
  def LUI   : ToyFormatU<OPC_LUI.Opcode,   "lui">;
  def AUIPC : ToyFormatU<OPC_AUPIC.Opcode, "auipc">;
  def JAL   : ToyFormatJ<OPC_JAL.Opcode,   "jal">;
  def JALR  : ToyFormatI<OPC_JALR.Opcode, 0b000, "jal">;

  def BEQ  : ToyFormatB<OPC_BRANCH.Opcode, 0b000, "beq">;
  def BNE  : ToyFormatB<OPC_BRANCH.Opcode, 0b001, "bne">;
  def BLT  : ToyFormatB<OPC_BRANCH.Opcode, 0b100, "blt">;
  def BGE  : ToyFormatB<OPC_BRANCH.Opcode, 0b101, "bge">;
  def BLTU : ToyFormatB<OPC_BRANCH.Opcode, 0b110, "bltu">;
  def BGEU : ToyFormatB<OPC_BRANCH.Opcode, 0b111, "bgeu">;
}

def LB  : ToyFormatILoad<0b000, "lb">;
def LH  : ToyFormatILoad<0b001, "lh">;
def LW  : ToyFormatILoad<0b010, "lw">;
def LBU : ToyFormatILoad<0b100, "lbu">;
def LHU : ToyFormatILoad<0b101, "lhu">;

def SB : ToyFormatSStore<0b000, "sb">;
def SH : ToyFormatSStore<0b001, "sh">;
def SW : ToyFormatSStore<0b010, "sw">;

let mayLoad = 0, mayStore = 0, hasSideEffects = 0 in {
  def ADDI  : ToyFormatI<OPC_OP_IMM.Opcode, 0b000, "addi">;
  def SLTI  : ToyFormatI<OPC_OP_IMM.Opcode, 0b010, "slti">;
  def SLTIU : ToyFormatI<OPC_OP_IMM.Opcode, 0b011, "sltiu">;
  def XORI  : ToyFormatI<OPC_OP_IMM.Opcode, 0b100, "xori">;
  def ORI   : ToyFormatI<OPC_OP_IMM.Opcode, 0b110, "ori">;
  def ANDI  : ToyFormatI<OPC_OP_IMM.Opcode, 0b111, "andi">;

  def SLLI  : ToyFormatIShift<OPC_OP_IMM.Opcode, 0b001, 0b0000000, "addi">;
  def SRLI  : ToyFormatIShift<OPC_OP_IMM.Opcode, 0b101, 0b0000000, "addi">;
  def SRAI  : ToyFormatIShift<OPC_OP_IMM.Opcode, 0b101, 0b0100000, "addi">;

  def ADD  : ToyFormatR<OPC_OP.Opcode, 0b000, 0b0000000, "add">;
  def SUB  : ToyFormatR<OPC_OP.Opcode, 0b000, 0b0100000, "sub">;
  def SLL  : ToyFormatR<OPC_OP.Opcode, 0b001, 0b0000000, "sll">;
  def SLT  : ToyFormatR<OPC_OP.Opcode, 0b010, 0b0000000, "slt">;
  def SLTU : ToyFormatR<OPC_OP.Opcode, 0b011, 0b0000000, "sltu">;
  def XOR  : ToyFormatR<OPC_OP.Opcode, 0b100, 0b0000000, "xor">;
  def SRL  : ToyFormatR<OPC_OP.Opcode, 0b101, 0b0000000, "srl">;
  def SRA  : ToyFormatR<OPC_OP.Opcode, 0b101, 0b0100000, "sra">;
  def OR   : ToyFormatR<OPC_OP.Opcode, 0b110, 0b0000000, "or">;
  def AND  : ToyFormatR<OPC_OP.Opcode, 0b111, 0b0000000, "and">;
}
// -------------------------------- RV32I/RV64I -------------------------------

// ---------------------------------- Pseudo ----------------------------------
include "ToyInstrPseudo.td"
// ---------------------------------- Pseudo ----------------------------------
