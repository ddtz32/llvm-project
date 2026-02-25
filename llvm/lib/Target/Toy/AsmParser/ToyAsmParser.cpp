#include "MCTargetDesc/ToyBaseInfo.h"
#include "MCTargetDesc/ToyInstPrinter.h"
#include "MCTargetDesc/ToyMCTargetDesc.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAsmMacro.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
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
  bool isToy64() const { return getSTI().hasFeature(Toy::Feature64Bit); }

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
  ParseStatus parseMemOpBaseRegister(OperandVector &Operands);
  ParseStatus parseFenceArg(OperandVector &Operands);

#define GET_ASSEMBLER_HEADER
#include "ToyGenAsmMatcher.inc"
#undef GET_ASSEMBLER_HEADER
};

struct ToyOperand final : public MCParsedAsmOperand {

  enum class KindTy {
    Token,
    Expression,
    Register,
    Fence,
  } Kind;

  struct ExprOp {
    const MCExpr *Expr;
    bool IsToy64;
  };

  union {
    StringRef Tok;
    ExprOp Expr;
    MCRegister Reg;
    unsigned Fence;
  };

  SMLoc StartLoc, EndLoc;

  explicit ToyOperand(KindTy Kind) : MCParsedAsmOperand(), Kind(Kind) {}

  bool isToken() const override { return Kind == KindTy::Token; }
  bool isImm() const override { return isExpr(); }
  bool isReg() const override { return Kind == KindTy::Register; }
  bool isMem() const override { llvm_unreachable("TODO"); }
  bool isExpr() const { return Kind == KindTy::Expression; }
  bool isToy64Expr() const {
    assert(isExpr() && "Invalid type access!");
    return Expr.IsToy64;
  }
  bool isFenceArg() const { return Kind == KindTy::Fence; }

  static bool evaluateConstantExpr(const MCExpr *Expr, int64_t &Imm) {
    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
      Imm = CE->getValue();
      return true;
    }
    return false;
  }

  static int64_t fixImmediateForToy32(int64_t Imm, bool IsToy64Imm) {
    if (!IsToy64Imm && isUInt<32>(Imm))
      return SignExtend64<32>(Imm);
    return Imm;
  }

  template <unsigned N> bool isSImm() const {
    if (!isExpr())
      return false;

    int64_t Imm;
    bool IsConstant = evaluateConstantExpr(getExpr(), Imm);
    return IsConstant && isInt<N>(fixImmediateForToy32(Imm, isToy64Expr()));
  }

  template <unsigned N> bool isSImmLsb0() const {
    if (!isExpr())
      return false;

    int64_t Imm;
    bool IsConstant = evaluateConstantExpr(getExpr(), Imm);
    return IsConstant &&
           isShiftedInt<N - 1, 1>(fixImmediateForToy32(Imm, isToy64Expr()));
  }

  template <unsigned N> bool isUImm() const {
    if (!isExpr())
      return false;

    int64_t Imm;
    bool IsConstant = evaluateConstantExpr(getExpr(), Imm);
    return IsConstant && isUInt<N>(fixImmediateForToy32(Imm, isToy64Expr()));
  }

  StringRef getToken() const {
    assert(isToken() && "Invalid type access!");
    return Tok;
  }

  MCRegister getReg() const override {
    assert(isReg() && "Invalid type access!");
    return Reg;
  }

  const MCExpr *getExpr() const {
    assert(isExpr() && "Invalid type access!");
    return Expr.Expr;
  }

  unsigned getFence() const {
    assert(isFenceArg() && "Invalid type access!");
    return Fence;
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
      OS << ' ' << (isToy64Expr() ? "toy64" : "toy32") << '>';
      break;
    case KindTy::Register:
      // TODO
      OS << "<reg: " << (Reg ? ToyInstPrinter::getRegisterName(Reg) : "noreg")
         << " (" << Reg.id() << ")>";
      break;
    case KindTy::Fence:
      OS << "<fence: " << getFence() << '>';
      break;
    }
  }

  static void addExpr(MCInst &Inst, const MCExpr *Expr, bool IsToy64) {
    int64_t Imm;
    bool IsConstant = evaluateConstantExpr(Expr, Imm);
    if (IsConstant)
      Inst.addOperand(MCOperand::createImm(fixImmediateForToy32(Imm, IsToy64)));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  // convert asm parsed operands to MCInst
  // used by tablgen in ToyGenAsmMatcher.inc
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getExpr(), isToy64Expr());
  }

  void addFenceArgOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createImm(getFence()));
  }

  static std::unique_ptr<ToyOperand> createToken(StringRef Tok, SMLoc Loc) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Token);
    Op->Tok = Tok;
    Op->StartLoc = Loc;
    Op->EndLoc = Loc;
    return Op;
  }

  static std::unique_ptr<ToyOperand>
  createExpr(const MCExpr *Expr, bool IsToy64, SMLoc S, SMLoc E) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Expression);
    Op->Expr.Expr = Expr;
    Op->Expr.IsToy64 = IsToy64;
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

  static std::unique_ptr<ToyOperand> createFenceArg(unsigned Val, SMLoc S,
                                                    SMLoc E) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Fence);
    Op->Fence = Val;
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
  MCInst Inst;
  FeatureBitset MissingFeatures;
  auto Result = MatchInstructionImpl(Operands, Inst, ErrorInfo, MissingFeatures,
                                     MatchingInlineAsm);
  switch (Result) {
  default:
    llvm_unreachable("TODO");
  case Match_Success:
    Out.emitInstruction(Inst, getSTI());
    return false;
  }
}

bool ToyAsmParser::parseOperand(OperandVector &Operands) {
  // parse register
  if (parseRegister(Operands).isSuccess())
    return false;

  // parse expression, now this is an immedidate
  if (parseExpression(Operands).isSuccess()) {
    // immediate may be followed by '(' + register + ')'
    if (getLexer().getTok().is(AsmToken::LParen))
      return !parseMemOpBaseRegister(Operands).isSuccess();
    return false;
  }

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

  Operands.push_back(ToyOperand::createExpr(Expr, isToy64(), S, E));
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

ParseStatus ToyAsmParser::parseMemOpBaseRegister(OperandVector &Operands) {
  if (getParser().parseToken(AsmToken::LParen, "expected '('"))
    return ParseStatus::Failure;
  Operands.push_back(ToyOperand::createToken("(", getLoc()));

  if (!parseRegister(Operands).isSuccess())
    return ParseStatus::Failure;

  if (getParser().parseToken(AsmToken::RParen, "expected ')'"))
    return ParseStatus::Failure;
  Operands.push_back(ToyOperand::createToken(")", getLoc()));

  return ParseStatus::Success;
}

ParseStatus ToyAsmParser::parseFenceArg(OperandVector &Operands) {
  const AsmToken &Token = getLexer().getTok();
  if (Token.is(AsmToken::Integer)) {
    if (Token.getIntVal() != 0)
      goto ParseFail;
    Operands.push_back(ToyOperand::createFenceArg(0, getLoc(), getEndLoc()));
    getLexer().Lex(); // eat 0
    return ParseStatus::Success;
  }

  if (Token.is(AsmToken::Identifier)) {
    StringRef Str = Token.getIdentifier();
    unsigned Imm = 0;
    bool Valid = true;
    char Prev = '\0';
    for (char Cur : Str) {
      switch (Cur) {
      default:
        Valid = false;
        break;
      case 'i':
        Imm |= static_cast<unsigned>(ToyFenceField::I);
        break;
      case 'o':
        Imm |= static_cast<unsigned>(ToyFenceField::O);
        break;
      case 'r':
        Imm |= static_cast<unsigned>(ToyFenceField::R);
        break;
      case 'w':
        Imm |= static_cast<unsigned>(ToyFenceField::W);
        break;
      }
      if (Cur < Prev)
        Valid = false;
      if (!Valid)
        break;
      Prev = Cur;
    }

    if (!Valid)
      goto ParseFail;

    Operands.push_back(ToyOperand::createFenceArg(Imm, getLoc(), getEndLoc()));
    getLexer().Lex(); // eat iorw
    return ParseStatus::Success;
  }
ParseFail:
  return TokError("operand must be formed of letters selected in-order from "
                  "'iorw' or be 0");
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyAsmParser() {
  RegisterMCAsmParser<ToyAsmParser> X(getTheToy32Target());
  RegisterMCAsmParser<ToyAsmParser> Y(getTheToy64Target());
}
