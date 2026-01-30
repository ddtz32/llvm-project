class ToyInst<dag outs, dag ins, string opcodestr, string argstr> : Instruction {
  let Namespace = "Toy";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = opcodestr # !if(!empty(argstr), "", "\t" # argstr);
}

class ToyFormatR<string opcodestr>
    : ToyInst<(outs GPR:$rd), (ins GPR:$rs1, GPR:$rs2), opcodestr,
              "$rd, $rs1, $rs2">;

let mayLoad = 0, mayStore = 0, hasSideEffects = 0 in
  def XXX : ToyFormatR<"test">;
