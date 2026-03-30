#include "MCTargetDesc/ToyBaseInfo.h"
#include "MCTargetDesc/ToyInstPrinter.h"
#include "MCTargetDesc/ToyMCAsmInfo.h"
#include "MCTargetDesc/ToyMCTargetDesc.h"
#include "MCTargetDesc/ToyMatInt.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAsmMacro.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/SMLoc.h"
#include <cstdint>

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

  bool parseRegister(MCRegister &Reg, SMLoc &S, SMLoc &E) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &S, SMLoc &E) override;

  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  ParseStatus parseDirective(AsmToken DirectiveID) override;
  bool parseDirectiveInsn(SMLoc NmaeLoc);

  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool processInstruction(MCInst &Inst, MCStreamer &Out);
  void emitToStreamer(MCInst &Inst, MCStreamer &Out);
  void emitLoadImm(MCRegister Reg, int64_t Imm, MCStreamer &Out);

  bool parseOperand(OperandVector &Operands, StringRef Mnemonic);
  ParseStatus parseExpression(OperandVector &Operands);
  ParseStatus parseRegister(OperandVector &Operands);
  ParseStatus parseMemOpBaseRegister(OperandVector &Operands);
  ParseStatus parseFenceArg(OperandVector &Operands);
  ParseStatus parseInsnDirectiveOpcode(OperandVector &Operands);
  ParseStatus parseOperandWithSpecifier(OperandVector &Operands);
  ParseStatus parseCSRSystemRegister(OperandVector &Operands);

  bool generateImmOutOfRangeError(SMLoc ErrorLoc, int64_t Lower, int64_t Upper,
                                  const Twine &Msg);

#define GET_ASSEMBLER_HEADER
#include "ToyGenAsmMatcher.inc"
#undef GET_ASSEMBLER_HEADER
};

int64_t fixImmediateForToy32(int64_t Imm, bool IsToy64Imm) {
  if (!IsToy64Imm && isUInt<32>(Imm))
    return SignExtend64<32>(Imm);
  return Imm;
}

struct ToyOperand final : public MCParsedAsmOperand {

  enum class KindTy {
    Token,
    Expression,
    Register,
    Fence,
    SystemRegister,
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
    const ToySysReg::SysReg *SysReg;
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
  bool isCSRSystemRegister() const { return Kind == KindTy::SystemRegister; }

  static bool evaluateConstantExpr(const MCExpr *Expr, int64_t &Imm) {
    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
      Imm = CE->getValue();
      return true;
    }
    return false;
  }

  static bool classsifySymbolRef(const MCExpr *Expr, Toy::Specifier &Kind) {
    Kind = ELF::R_TOY_NONE;
    if (const MCSpecifierExpr *SE = dyn_cast<MCSpecifierExpr>(Expr)) {
      Kind = SE->getSpecifier();
      Expr = SE->getSubExpr();
    }

    MCValue Res;
    if (Expr->evaluateAsRelocatable(Res, nullptr))
      return Res.getSpecifier() == ELF::R_TOY_NONE;
    return false;
  }

  static bool isSymbolDiff(const MCExpr *Expr) {
    MCValue Res;
    if (Expr->evaluateAsRelocatable(Res, nullptr)) {
      return Res.getSpecifier() == ELF::R_TOY_NONE && Res.getAddSym() &&
             Res.getSubSym();
    }
    return false;
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

  bool isSImm12LO() const {
    if (isSImm<12>())
      return true;

    Toy::Specifier Kind;
    return isExpr() && classsifySymbolRef(getExpr(), Kind) &&
           (Kind == ELF::R_TOY_LO12 || Kind == ELF::R_TOY_PCREL_LO12);
  }

  bool isUImm20AUIPC() const {
    if (isUImm<20>())
      return true;

    Toy::Specifier Kind;
    return isExpr() && classsifySymbolRef(getExpr(), Kind) &&
           Kind == ELF::R_TOY_PCREL_HI20;
  }

  bool isImm32LI() const {
    if (isImm32LA())
      return true;
    return isExpr() && isSymbolDiff(getExpr());
  }

  bool isImm32LA() const {
    if (!isExpr())
      return false;

    int64_t Imm;
    bool IsConstant = evaluateConstantExpr(getExpr(), Imm);
    // The immediate here can be 32 or 64 bit
    return IsConstant || (isToy64Expr() || isInt<32>(Imm) || isUInt<32>(Imm));
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

  StringRef getSysReg() const {
    assert(isCSRSystemRegister() && "Invalid type access!");
    return SysReg->Name;
  }

  unsigned getSysRegEncoding() const {
    assert(isCSRSystemRegister() && "Invalid type access!");
    return SysReg->Encoding;
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
      OS << "<reg: "
         << (getReg() ? ToyInstPrinter::getRegisterName(getReg()) : "noreg")
         << " (" << getReg().id() << ")>";
      break;
    case KindTy::Fence:
      OS << "<fence: " << getFence() << '>';
      break;
    case KindTy::SystemRegister:
      OS << "<sysreg: " << getSysReg() << " (" << getSysRegEncoding() << ")>";
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

  void addCSRSystemRegisterOperands(MCInst &Inst, unsigned N) {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createImm(SysReg->Encoding));
  }

  static std::unique_ptr<ToyOperand> createToken(StringRef Tok, SMLoc L) {
    auto Op = std::make_unique<ToyOperand>(KindTy::Token);
    Op->Tok = Tok;
    Op->StartLoc = L;
    Op->EndLoc = L;
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

  static std::unique_ptr<ToyOperand>
  createSysReg(const ToySysReg::SysReg *SysReg, SMLoc S, SMLoc E) {
    auto Op = std::make_unique<ToyOperand>(KindTy::SystemRegister);
    Op->SysReg = SysReg;
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

bool ToyAsmParser::parseRegister(MCRegister &Reg, SMLoc &S, SMLoc &E) {
  llvm_unreachable("TODO");
}

ParseStatus ToyAsmParser::tryParseRegister(MCRegister &Reg, SMLoc &S,
                                           SMLoc &E) {
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
  if (parseOperand(Operands, Name))
    return true;

  // parse ',' + operand
  while (parseOptionalToken(AsmToken::Comma)) {
    if (parseOperand(Operands, Name))
      return true;
  }

  return getParser().parseEOL();
}

ParseStatus ToyAsmParser::parseDirective(AsmToken DirectiveID) {
  StringRef IDVal = DirectiveID.getIdentifier();
  if (IDVal == ".insn")
    return parseDirectiveInsn(DirectiveID.getLoc());

  return ParseStatus::NoMatch;
}

// TODO: length must be 4, and value must 32 bit value
// parseDirectiveInsn
// ::= .insn [ value ]
// ::= .insn [ length, value ]
// ::= .insn [ format encoding, (operand (, operand)*) ]
bool ToyAsmParser::parseDirectiveInsn(SMLoc NameLoc) {
  MCAsmParser &Parser = getParser();

  StringRef Format;
  SMLoc ErrorLoc = Parser.getTok().getLoc();
  if (Parser.parseIdentifier(Format)) {
    // .insn [ value ]
    int64_t Value;
    if (Parser.parseAbsoluteExpression(Value))
      return true;
    if (Parser.parseOptionalToken(AsmToken::Comma)) {
      // .insn [ length, value ]
      int64_t Length = Value;
      if (Length != 4)
        return Error(ErrorLoc, "instruction lengths must be 4");
      if (Parser.parseAbsoluteExpression(Value))
        return true;
    }

    if (!isUIntN(32, Value))
      return Error(ErrorLoc, "encoding value does not fit into instruction");

    Parser.getStreamer().emitInstruction(MCInstBuilder(Toy::Insn).addImm(Value),
                                         getSTI());
    return false;
  }

  bool IsValid = StringSwitch<bool>(Format)
                     .Cases({"r", "i", "s", "b", "u", "j"}, true)
                     .Default(false);
  if (!IsValid)
    return Error(ErrorLoc, "invalid insn instruction foramt");

  std::string FormatName = (".insn_" + Format).str();

  ParseInstructionInfo Info;
  SmallVector<std::unique_ptr<MCParsedAsmOperand>> Operands;
  if (parseInstruction(Info, FormatName, NameLoc, Operands))
    return true;

  unsigned Opcode;
  uint64_t ErrorInfo;
  return matchAndEmitInstruction(NameLoc, Opcode, Operands,
                                 Parser.getStreamer(), ErrorInfo, false);
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
    return processInstruction(Inst, Out);
  }
}

bool ToyAsmParser::processInstruction(MCInst &Inst, MCStreamer &Out) {
  switch (Inst.getOpcode()) {
  default:
    break;
  case Toy::PsdudoLI:
  case Toy::PsdudoLAImm:
  case Toy::PsdudoLLAImm: {
    MCRegister Reg = Inst.getOperand(0).getReg();
    const MCOperand &Op1 = Inst.getOperand(1);
    assert((Op1.isExpr() || Op1.isImm()) &&
           "li only support expression or immediate");
    if (Op1.isExpr()) {
      // 这里的表达式 12 bit 一定能表示?
      emitToStreamer(
          MCInstBuilder(Toy::ADDI).addReg(Reg).addReg(Toy::X0).addExpr(
              Op1.getExpr()),
          Out);
      return false;
    }
    int64_t Imm = fixImmediateForToy32(Op1.getImm(), isToy64());
    emitLoadImm(Reg, Imm, Out);
    return false;
  }
  }
  emitToStreamer(Inst, Out);
  return false;
}

void ToyAsmParser::emitToStreamer(MCInst &Inst, MCStreamer &Out) {
  Out.emitInstruction(Inst, getSTI());
}

void ToyAsmParser::emitLoadImm(MCRegister Reg, int64_t Imm, MCStreamer &Out) {
  SmallVector<MCInst, 8> Seq;
  ToyMatInt::generateMCInstSeq(Imm, getSTI(), Reg, Seq);
  for (auto &Inst : Seq)
    emitToStreamer(Inst, Out);
}

bool ToyAsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic) {
  ParseStatus Result = MatchOperandParserImpl(Operands, Mnemonic);
  if (Result.isSuccess())
    return false;
  if (Result.isFailure())
    return true;

  // parse register
  if (parseRegister(Operands).isSuccess())
    return false;

  // parse expression, now this is an immediate
  if (parseExpression(Operands).isSuccess()) {
    // immediate may be followed by '(' + register + ')'
    if (getLexer().getTok().is(AsmToken::LParen))
      return !parseMemOpBaseRegister(Operands).isSuccess();
    return false;
  }

  // immediate is optional, such as lb x10, (x11)
  if (getLexer().getTok().is(AsmToken::LParen))
    return !parseMemOpBaseRegister(Operands).isSuccess();

  return true;
}

ParseStatus ToyAsmParser::parseExpression(OperandVector &Operands) {
  SMLoc S = getLoc(), E;
  const MCExpr *Expr;

  switch (getLexer().getTok().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::Minus:
  case AsmToken::Integer:
  case AsmToken::Identifier:
    if (getParser().parseExpression(Expr, E))
      return ParseStatus::Failure;
    break;
  case AsmToken::Percent:
    return parseOperandWithSpecifier(Operands);
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

ParseStatus ToyAsmParser::parseInsnDirectiveOpcode(OperandVector &Operands) {
  SMLoc S = getLoc(), E;
  const MCExpr *Expr;
  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::Integer:
    if (getParser().parseExpression(Expr, E))
      return ParseStatus::Failure;

    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
      int64_t Imm = CE->getValue();
      if (isUInt<7>(Imm)) {
        Operands.push_back(ToyOperand::createExpr(Expr, isToy64(), S, E));
        return ParseStatus::Success;
      }
    }
    break;
  case AsmToken::Identifier: {
    StringRef Name;
    if (getParser().parseIdentifier(Name))
      return ParseStatus::Failure;

    const ToyInsnOpcode::ToyOpcode *Opcode =
        ToyInsnOpcode::lookupToyOpcodeByName(Name);
    if (Opcode && isUInt<7>(Opcode->Value)) {
      const MCExpr *Expr = MCConstantExpr::create(Opcode->Value, getContext());
      E = SMLoc::getFromPointer(S.getPointer() + Name.size());
      Operands.push_back(ToyOperand::createExpr(Expr, isToy64(), S, E));
      return ParseStatus::Success;
    }
  }
  }

  return generateImmOutOfRangeError(
      S, 0, (1 << 7) - 1,
      "opcode must be a valid opcode or an immediate in the range");
}

ParseStatus ToyAsmParser::parseOperandWithSpecifier(OperandVector &Operands) {
  SMLoc S = getLoc(), E;

  if (parseToken(AsmToken::Percent, "expected '%' relocation sepcifier"))
    return ParseStatus::Failure;

  if (getLexer().getKind() != AsmToken::Identifier)
    return TokError("expected '%' relocation sepcifier");
  StringRef Identifier = getParser().getTok().getIdentifier();
  Toy::Specifier Spec = Toy::parseSpeciferName(Identifier);
  if (!Spec)
    return TokError("invalid relocation sepcifier");

  getParser().Lex(); // Eat the identifier
  if (parseToken(AsmToken::LParen, "expected '('"))
    return ParseStatus::Failure;

  const MCExpr *SubExpr;
  if (getParser().parseParenExpression(SubExpr, E))
    return ParseStatus::Failure;

  const MCExpr *Expr = MCSpecifierExpr::create(SubExpr, Spec, getContext(), S);
  Operands.push_back(ToyOperand::createExpr(Expr, isToy64(), S, E));
  return ParseStatus::Success;
}

ParseStatus ToyAsmParser::parseCSRSystemRegister(OperandVector &Operands) {
  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::Identifier: {
    StringRef Identifier;
    if (getParser().parseIdentifier(Identifier))
      return ParseStatus::Failure;

    const auto *SysReg = ToySysReg::lookupSysRegByName(Identifier);
    if (SysReg) {
      Operands.push_back(
          ToyOperand::createSysReg(SysReg, getLoc(), getEndLoc()));
      return ParseStatus::Success;
    }
    return Error(getLoc(), "invalid csr system register");
  }
  }
}

bool ToyAsmParser::generateImmOutOfRangeError(
    SMLoc ErrorLoc, int64_t Lower, int64_t Upper,
    const Twine &Msg = "immediate must be an integer in the range") {
  return Error(ErrorLoc, Msg + " [" + Twine(Lower) + ", " + Twine(Upper) + "]");
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyAsmParser() {
  RegisterMCAsmParser<ToyAsmParser> X(getTheToy32Target());
  RegisterMCAsmParser<ToyAsmParser> Y(getTheToy64Target());
}
