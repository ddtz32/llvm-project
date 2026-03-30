// RISV-V pseudo instructions
class Pseudo<dag outs, dag ins, string opcodestr, string argstr>
    : ToyInst<outs, ins, opcodestr, argstr> {
  let isPseudo = 1;
}

// TODO: Load and store address
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Size = 8 in {
  def PsdudoLAImm  : Pseudo<(outs GPR:$rd), (ins imm32_la:$imm), "la",
                            "$rd, $imm">;
  def PsdudoLLAImm : Pseudo<(outs GPR:$rd), (ins imm32_la:$imm), "lla",
                            "$rd, $imm">;
}

def : InstAlias<"lb $rd, ($rs1)",  (LB GPR:$rd, GPR:$rs1, 0)>;
def : InstAlias<"lh $rd, ($rs1)",  (LH GPR:$rd, GPR:$rs1, 0)>;
def : InstAlias<"lw $rd, ($rs1)",  (LW GPR:$rd, GPR:$rs1, 0)>;
def : InstAlias<"lbu $rd, ($rs1)", (LBU GPR:$rd, GPR:$rs1, 0)>;
def : InstAlias<"lhu $rd, ($rs1)", (LHU GPR:$rd, GPR:$rs1, 0)>;

def : InstAlias<"sb $rs2, ($rs1)", (SB GPR:$rs2, GPR:$rs1, 0)>;
def : InstAlias<"sh $rs2, ($rs1)", (SH GPR:$rs2, GPR:$rs1, 0)>;
def : InstAlias<"sw $rs2, ($rs1)", (SW GPR:$rs2, GPR:$rs1, 0)>;

def : InstAlias<"nop",          (ADDI X0,      X0,      0)>;

def : InstAlias<"li $rd, $imm", (ADDI GPR:$rd, X0,      simm12_lo:$imm)>;
// PsdudoLI may be mapped to lui+addi, so Size is 8
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Size = 8 in
def PsdudoLI : Pseudo<(outs GPR:$rd), (ins imm32_li:$imm), "li", "$rd, $imm">;

def : InstAlias<"mv $rd, $rs",  (ADDI GPR:$rd, GPR:$rs, 0)>;

def : InstAlias<"not $rd, $rs", (XORI GPR:$rd, GPR:$rs, -1)>;
def : InstAlias<"neg $rd, $rs", (SUB  GPR:$rd, X0,      GPR:$rs)>;
// TODO: negw, sext.b, sext.h, sext.w

def : InstAlias<"zext.b $rd, $rs", (ANDI GPR:$rd, GPR:$rs, 0xFF)>;
// TODO: zext.h, zext.w

def : InstAlias<"seqz $rd, $rs", (SLTIU GPR:$rd, GPR:$rs, 1)>;
def : InstAlias<"snez $rd, $rs", (SLTU  GPR:$rd, X0,      GPR:$rs)>;
def : InstAlias<"sltz $rd, $rs", (SLT   GPR:$rd, GPR:$rs, X0)>;
def : InstAlias<"sgtz $rd, $rs", (SLT   GPR:$rd, X0,      GPR:$rs)>;

def : InstAlias<"beqz $rs, $offset", (BEQ GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"bnez $rs, $offset", (BNE GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"blez $rs, $offset", (BGE X0, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"bgez $rs, $offset", (BGE GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"bltz $rs, $offset", (BLT GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"bgtz $rs, $offset", (BLT X0, GPR:$rs, simm13_lsb0:$offset)>;

def : InstAlias<"bgt  $rs, $rt, $offset",
                (BLT  GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"ble  $rs, $rt, $offset",
                (BGE  GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"bgtu $rs, $rt, $offset",
                (BLTU GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"bleu $rs, $rt, $offset",
                (BGEU GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;

def : InstAlias<"j    $offset", (JAL  X0, simm21_lsb0:$offset)>;
def : InstAlias<"jal  $offset", (JAL  X1, simm21_lsb0:$offset)>;
def : InstAlias<"jr   $rs",     (JALR X0, GPR:$rs, 0)>;
def : InstAlias<"jalr $rs",     (JALR X1, GPR:$rs, 0)>;
def : InstAlias<"ret",          (JALR X1, X1, 0)>;

// TODO: tail

def : InstAlias<"fence", (FENCE 0xF, 0xF)>;
def : InstAlias<"pause", (FENCE 0x1, 0x0)>; // 0x1 == w

def : InstAlias<"rdinstreth $rd", (CSRRS GPR:$rd, INSTRETH.Encoding, X0)>;
def : InstAlias<"rdcycleh $rd",   (CSRRS GPR:$rd, CYCLEH.Encoding,   X0)>;
def : InstAlias<"rdtimeh $rd",    (CSRRS GPR:$rd, TIMEH.Encoding,    X0)>;
