#include "MCTargetDesc/ToyInstPrinter.h"
#include "MCTargetDesc/ToyMCTargetDesc.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace {

class ToyAsmParser : public MCTargetAsmParser {
public:
  ToyAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
               const MCInstrInfo &MII, const MCTargetOptions &Options);

  // utility fucntions
  SMLoc getLoc() const { return getParser().getTok().getLoc(); }
  SMLoc getEndLoc() const { return getParser().getTok().getEndLoc(); }

  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;

  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool parseOperand(OperandVector &Operands);
  ParseStatus parseExpression(OperandVector &Operands);
  ParseStatus parseRegister(OperandVector &Operands);

#define GET_ASSEMBLER_HEADER
#include "ToyGenAsmMatcher.inc"
#undef GET_ASSEMBLER_HEADER
};

struct ToyOperand final : public MCParsedAsmOperand {

  enum class KindTy {
    Token,
    Expression,
    Register,
  } Kind;

  union {
    StringRef Tok;
    const MCExpr *Expr;
    MCRegister Reg;
  };

  SMLoc StartLoc, EndLoc;

  explicit ToyOperand(KindTy Kind) : MCParsedAsmOperand(), Kind(Kind) {}

  bool isToken() const override { return Kind == KindTy::Token; }
  bool isImm() const override {
    return isExpr() && dyn_cast<MCConstantExpr>(getExpr());
  }
  bool isReg() const override { return Kind == KindTy::Register; }
  bool isMem() const override { llvm_unreachable("TODO"); }
  bool isExpr() const { return Kind == KindTy::Expression; }

  StringRef getToken() const {
    assert(isToken() && "Invalid type access!");
    return Tok;
  }

  int64_t getImm() const {
    assert(isImm() && "Invalid type access!");
    return dyn_cast<MCConstantExpr>(getExpr())->getValue();
  }

  MCRegister getReg() const override {
    assert(isReg() && "Invalid type access!");
    return Reg;
  }

  const MCExpr *getExpr() const {
    assert(isExpr() && "Invalid type access!");
    return Expr;
  }

  SMLoc getStartLoc() const override { return StartLoc; }
  SMLoc getEndLoc() const override { return EndLoc; }

  void print(raw_ostream &OS, const MCAsmInfo &MAI) const override {
    switch (Kind) {
    case KindTy::Token:
      OS << "'" << getToken() << "'";
      break;
    case KindTy::Expression:
      assert(isImm() && "TODO");
      OS << "<imm: ";
      MAI.printExpr(OS, *getExpr());
      OS << ">";
      break;
    case KindTy::Register:
      // TODO
      OS << "<reg: " << (Reg ? ToyInstPrinter::getRegisterName(Reg) : "noreg")
         << " (" << Reg.id() << ")>";
      break;
    }
  }

  // used by tablgen in ToyGenAsmMatcher.inc
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  static std::unique_ptr<ToyOperand> createToken(StringRef Tok, SMLoc Loc) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Token);
    Op->Tok = Tok;
    Op->StartLoc = Loc;
    Op->EndLoc = Loc;
    return Op;
  }

  static std::unique_ptr<ToyOperand> createExpr(const MCExpr *Expr, SMLoc S,
                                                SMLoc E) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Expression);
    Op->Expr = Expr;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<ToyOperand> createReg(MCRegister Reg, SMLoc S,
                                               SMLoc E) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Register);
    Op->Reg = Reg;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }
};

} // namespace

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "ToyGenAsmMatcher.inc"

ToyAsmParser::ToyAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                           const MCInstrInfo &MII,
                           const MCTargetOptions &Options)
    : MCTargetAsmParser(Options, STI, MII) {}

bool ToyAsmParser::parseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                 SMLoc &EndLoc) {
  llvm_unreachable("TODO");
}

ParseStatus ToyAsmParser::tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,

                                           SMLoc &EndLoc) {
  llvm_unreachable("TODO");
}

// `parseInstruction` is used to parse a single asm instruction
bool ToyAsmParser::parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                    SMLoc NameLoc, OperandVector &Operands) {
  // first operand is the opcode string of instruction
  Operands.push_back(ToyOperand::createToken(Name, NameLoc));

  // if this instruciton has no operand, then finish
  if (getLexer().is(AsmToken::EndOfStatement)) {
    getParser().Lex(); // eat the EndOfStatement
    return false;
  }

  // parse first operand
  if (parseOperand(Operands))
    return true;

  // parse ',' + operand
  while (parseOptionalToken(AsmToken::Comma)) {
    if (parseOperand(Operands))
      return true;
  }

  return getParser().parseEOL();
}

bool ToyAsmParser::matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                           OperandVector &Operands,
                                           MCStreamer &Out, uint64_t &ErrorInfo,
                                           bool MatchingInlineAsm) {
  llvm_unreachable("TODO");
}

bool ToyAsmParser::parseOperand(OperandVector &Operands) {
  // parse register
  if (parseRegister(Operands).isSuccess())
    return false;

  // parse expression, now this is an immedidate
  if (parseExpression(Operands).isSuccess())
    return false;

  return true;
}

ParseStatus ToyAsmParser::parseExpression(OperandVector &Operands) {
  SMLoc S = getLoc(), E;
  const MCExpr *Expr;

  switch (getLexer().getTok().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::Integer:
    if (getParser().parseExpression(Expr, E))
      return ParseStatus::Failure;
    break;
  }

  Operands.push_back(ToyOperand::createExpr(Expr, S, E));
  return ParseStatus::Success;
}

ParseStatus ToyAsmParser::parseRegister(OperandVector &Operands) {
  const AsmToken &Token = getLexer().getTok();
  if (Token.isNot(AsmToken::Identifier))
    return ParseStatus::NoMatch;

  StringRef Name = Token.getIdentifier();
  MCRegister Reg = MatchRegisterName(Name);
  if (!Reg)
    // riscv registers also have an ABI name
    Reg = MatchRegisterAltName(Name);

  if (!Reg)
    return ParseStatus::NoMatch;

  Operands.push_back(ToyOperand::createReg(Reg, getLoc(), getEndLoc()));
  getLexer().Lex(); // eat register
  return ParseStatus::Success;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyAsmParser() {
  RegisterMCAsmParser<ToyAsmParser> X(getTheToy32Target());
  RegisterMCAsmParser<ToyAsmParser> Y(getTheToy64Target());
}
