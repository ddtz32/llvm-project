// RISV-V pseudo instructions
def : InstAlias<"beqz $rs, $offset", (BEQ GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"bnez $rs, $offset", (BNE GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"blez $rs, $offset", (BGE X0, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"bgez $rs, $offset", (BGE GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"blez $rs, $offset", (BLT GPR:$rs, X0, simm13_lsb0:$offset)>;
def : InstAlias<"bgtz $rs, $offset", (BLT X0, GPR:$rs, simm13_lsb0:$offset)>;

def : InstAlias<"bgt  $rs, $rt, $offset", (BLT  GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"ble  $rs, $rt, $offset", (BGE  GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"bgtu $rs, $rt, $offset", (BLTU GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;
def : InstAlias<"bleu $rs, $rt, $offset", (BGEU GPR:$rt, GPR:$rs, simm13_lsb0:$offset)>;

def : InstAlias<"j    $offset", (JAL  X0, simm21_lsb0:$offset)>;
def : InstAlias<"jal  $offset", (JAL  X1, simm21_lsb0:$offset)>;
def : InstAlias<"jr   $rs",     (JALR X0, GPR:$rs, 0)>;
def : InstAlias<"jalr $rs",     (JALR X1, GPR:$rs, 0)>;
def : InstAlias<"ret",          (JALR X1, X1, 0)>;
