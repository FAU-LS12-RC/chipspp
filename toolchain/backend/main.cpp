#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/CodeGen/TargetPassConfig.h>
#include <llvm/CodeGen/TargetRegisterInfo.h>
#include <llvm/CodeGen/TargetInstrInfo.h>
#include <llvm/CodeGen/SelectionDAGISel.h>
#include <llvm/CodeGen/TargetLowering.h>
#include <llvm/CodeGen/TargetSubtargetInfo.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/MC/MCAsmInfoELF.h>
#include <llvm/MC/MCStreamer.h>
#include <llvm/MC/MCInstPrinter.h>
#include <llvm/CodeGen/TargetLoweringObjectFileImpl.h>
#include <llvm/CodeGen/TargetFrameLowering.h>
#include <llvm/CodeGen/CallingConvLower.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/CodeGen/ScoreboardHazardRecognizer.h>
#include <llvm/CodeGen/ScheduleDAG.h>
#include <llvm/MC/MCExpr.h>
#include <llvm/MC/MCContext.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace llvm;

std::ofstream stackSizesOut;

//#define CHAR_SIZE 32
#define CHAR_SIZE 8

int scaledOffset(int offset) {
#if CHAR_SIZE == 32
  return offset >> 2;
#else
  return offset;
#endif
}

//TODO not 32-bit address compatible
int roundedToAlignment(int64_t size) {
  return (size + 3) & ~3;
}

static Target chips_target;

namespace ChipsISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  CMP,
  SETCC,
  BRCOND,
  RET_FLAG,
  CALL,
  ADDR_WRAPPER,
  LONG_TO_DOUBLE,
  DOUBLE_TO_LONG
};
}

class ChipsELFTargetObjectFile : public TargetLoweringObjectFileELF {
public:
  ChipsELFTargetObjectFile() : TargetLoweringObjectFileELF() { }

  void Initialize(MCContext &Ctx, const TargetMachine &TM) override {
    TargetLoweringObjectFileELF::Initialize(Ctx, TM);
    InitializeELF(TM.Options.UseInitArray);
  }
};

class ChipsSubtarget;
//TODO put everything into llvm namespace?
namespace llvm {
class ChipsFrameLowering : public TargetFrameLowering {
private:
  ChipsSubtarget const& subtarget;
public:
  ChipsFrameLowering(ChipsSubtarget const& subtarget) :
    TargetFrameLowering(TargetFrameLowering::StackGrowsUp, 8, 0),
    subtarget(subtarget) { }

  void emitPrologue(MachineFunction& func, MachineBasicBlock& block)
    const override;
  void emitEpilogue(MachineFunction& func, MachineBasicBlock& block)
    const override;
  
  bool hasFP(MachineFunction const& func) const {
    return false;
  }

  bool hasReservedCallFrame(MachineFunction const& func) const {
    return !func.getFrameInfo().hasVarSizedObjects();
  }

  MachineBasicBlock::iterator eliminateCallFramePseudoInstr(
      MachineFunction& func, MachineBasicBlock& block,
      MachineBasicBlock::iterator instrs) const override {
//    if (!hasReservedCallFrame(MF)) {
//      MachineInstr &MI = *I;
//      int Size = MI.getOperand(0).getImm();
//      if (MI.getOpcode() == SP::ADJCALLSTACKDOWN)
//        Size = -Size;
//
//      if (Size)
//        emitSPAdjustment(MF, MBB, I, Size, SP::ADDrr, SP::ADDri);
//    }
    return block.erase(instrs);
  }

  int getFrameIndexReference(MachineFunction const& func, int frameIndex,
      unsigned& frameReg) const override;
};
}

#define GET_REGINFO_HEADER
#define GET_REGINFO_TARGET_DESC
#define GET_REGINFO_ENUM
#define GET_REGINFO_MC_DESC
#include "ChipsGenRegisterInfo.inc"
#include "ChipsGenCallingConv.inc"
struct ChipsRegisterInfo : public ChipsGenRegisterInfo {
  ChipsRegisterInfo() : ChipsGenRegisterInfo(
      /*returnAddressReg=*/Chips::R6) { }

  MCPhysReg const* getCalleeSavedRegs(MachineFunction const* func) 
    const override {
    return CSR_SaveList;
  }
  uint32_t const* getCallPreservedMask(MachineFunction const& func,
      CallingConv::ID conv) const override {
    return CSR_RegMask;
  }

  BitVector getReservedRegs(MachineFunction const& func) const override {
    BitVector ret(getNumRegs());
    ret.set(Chips::R12);
    ret.set(Chips::R3);
    //ret.set(Chips::R4);
    ret.set(Chips::R6);
    return ret;
  }

  TargetRegisterClass const* getPointerRegClass(MachineFunction const& func,
      unsigned kind) const override {
    return &Chips::IntRegsRegClass;
  }

  bool enableMultipleCopyHints() const override {
    return true;
  }

  void eliminateFrameIndex(MachineBasicBlock::iterator instrs,
      int SPAdj, unsigned FIOperandNum,
      RegScavenger* RS = nullptr) const override;

  unsigned getFrameRegister(MachineFunction const& func) const override {
    return Chips::R3; //Chips::R4
  }

  bool canRealignStack(MachineFunction const& func) const override {
    if(!TargetRegisterInfo::canRealignStack(func))
      return false;

    if(getFrameLowering(func)->hasReservedCallFrame(func))
      return true;

    return false;
  }

  bool trackLivenessAfterRegAlloc(MachineFunction const& func) const {
    return true;
  }
};

//TODO in ChipsISelLowering.cpp
class ChipsTargetLowering : public TargetLowering {
private:
  ChipsSubtarget const& subtarget;

public:
  ChipsTargetLowering(TargetMachine const& tm,
      ChipsSubtarget const& subtarget);

#if CHAR_SIZE == 32
  void computeKnownBitsForFrameIndex(SDValue const op, KnownBits& known,
      APInt const& demandedElts, SelectionDAG const& dag,
      unsigned depth = 0) const override {
    return;
  }
#endif

  SDValue LowerFormalArguments(SDValue chain, CallingConv::ID conv,
      bool isVarArg, SmallVectorImpl<ISD::InputArg> const& ins, SDLoc const& dl,
      SelectionDAG& dag, SmallVectorImpl<SDValue>& inVals) const override;
  SDValue LowerReturn(SDValue chain, CallingConv::ID conv, bool isVarArg,
      SmallVectorImpl<ISD::OutputArg> const& outs,
      SmallVectorImpl<SDValue> const& outVals,
      SDLoc const& dl, SelectionDAG& dag) const override {
    MachineFunction& func = dag.getMachineFunction();
    //MachineRegisterInfo& regInfo = func.getRegInfo();
    //SparcMachineFunctionInfo *FuncInfo = func.getInfo<ChipsMachineFunctionInfo>();

    SmallVector<CCValAssign, 16> rvLocs;
    CCState ccInfo(conv, isVarArg, func, rvLocs, *dag.getContext());
    ccInfo.AnalyzeReturn(outs, RetCC_Chips);

    SDValue glue;
    SmallVector<SDValue, 4> retOps(1, chain);

    for (unsigned i = 0, e = rvLocs.size(); i < e; ++i) {
      SDValue val = outVals[i];
      CCValAssign& loc = rvLocs[i];
      assert(loc.isRegLoc() && "Can only return in registers!");

      //TODO handle CCValAssign::BCvt
      chain = dag.getCopyToReg(chain, dl, loc.getLocReg(), val, glue);

      // Guarantee that all emitted copies are stuck together.
      glue = chain.getValue(1);
      retOps.push_back(dag.getRegister(loc.getLocReg(), loc.getLocVT()));
    }

    retOps[0] = chain;

    if(glue.getNode()) {
      retOps.push_back(glue);
    }

    return dag.getNode(ChipsISD::RET_FLAG, dl, MVT::Other, retOps);
  }

  void createByvalArgCopies(SelectionDAG& dag, MachineFrameInfo& frameInfo,
      SmallVectorImpl<ISD::OutputArg>& outs, SmallVectorImpl<SDValue>& outVals,
      SDValue& chain, SDLoc& dl, SmallVector<SDValue, 8>& byValArgs) const {
    for(unsigned i = 0, e = outs.size(); i != e; ++i) {
      ISD::ArgFlagsTy flags = outs[i].Flags;
      if(!flags.isByVal())
        continue;

      SDValue outVal = outVals[i];
      unsigned size = flags.getByValSize();
      unsigned align = flags.getByValAlign();

      if(size > 0u) {
        int frameIndex = frameInfo.CreateStackObject(size, align, false);
        SDValue fiNode = dag.getFrameIndex(frameIndex,
            getPointerTy(dag.getDataLayout()));
        SDValue sizeNode = dag.getConstant(size, dl, MVT::i32);

        chain = dag.getMemcpy(chain, dl, fiNode, outVal, sizeNode, align,
            /*isVolatile=*/false, /*AlwaysInline=*/true, /*isTailCall=*/false,
            MachinePointerInfo(), MachinePointerInfo());
        byValArgs.push_back(fiNode);
      } else {
        SDValue nullVal;
        byValArgs.push_back(nullVal);
      }
    }
  }

  SDValue extendOutgoingArgIfNecessary(SelectionDAG& dag, SDLoc& dl,
      CCValAssign& argLoc, SDValue arg) const {
    switch(argLoc.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: break;
    case CCValAssign::SExt:
      return dag.getNode(ISD::SIGN_EXTEND, dl, argLoc.getLocVT(), arg);
    case CCValAssign::ZExt:
      return dag.getNode(ISD::ZERO_EXTEND, dl, argLoc.getLocVT(), arg);
    case CCValAssign::AExt:
      return dag.getNode(ISD::ANY_EXTEND, dl, argLoc.getLocVT(), arg);
    case CCValAssign::BCvt:
      return dag.getNode(ISD::BITCAST, dl, argLoc.getLocVT(), arg);
    }

    return arg;
  }

  SDValue LowerCall(TargetLowering::CallLoweringInfo& loweringInfo,
      SmallVectorImpl<SDValue>& inVals) const override;

  char const* getTargetNodeName(unsigned opcode) const {
    switch ((ChipsISD::NodeType)opcode) {
      case ChipsISD::SETCC: return "ChipsISD::SETCC";
      case ChipsISD::BRCOND: return "ChipsISD::BRCOND";
      case ChipsISD::CMP: return "ChipsISD::CMP";
      case ChipsISD::RET_FLAG: return "ChipsISD::RET_FLAG";
      case ChipsISD::CALL: return "ChipsISD::CALL";
      case ChipsISD::ADDR_WRAPPER: return "ChipsISD::ADDR_WRAPPER";
      default: return "Unknown Chips-specific node";
    }
  }

  void ReplaceNodeResults(SDNode* node, SmallVectorImpl<SDValue>& results,
      SelectionDAG& dag) const override {
    SDLoc dl(node);

//    RTLIB::Libcall libCall = RTLIB::UNKNOWN_LIBCALL;

    switch(node->getOpcode()) {
    default:
      llvm_unreachable("Do not know how to custom type legalize this operation!");

    case ISD::FP_TO_SINT:
//    case ISD::FP_TO_UINT:
      results.push_back(lowerFPToSInt(SDValue(node, 0), dag));
      return;
    }
  }

  SDValue LowerOperation(SDValue op, SelectionDAG& dag) const override {
    switch(op.getOpcode()) {
      case ISD::BRCOND: return lowerBrcond(op, dag);
      case ISD::GlobalAddress:
      case ISD::ConstantPool:
      case ISD::BlockAddress: return lowerAddress(op, dag);
      case ISD::BITCAST: return lowerBitcast(op, dag);
      case ISD::STORE: return lowerStore(op, dag);
      case ISD::LOAD: return lowerLoad(op, dag);
      case ISD::SINT_TO_FP: return lowerSIntToFP(op, dag);
      case ISD::FP_TO_SINT: return lowerFPToSInt(op, dag);
    }
    llvm_unreachable("Not lowerable");
  }

  SDValue lowerSIntToFP(SDValue op, SelectionDAG& dag) const {
    SDLoc dl(op);
    EVT integerType = op.getOperand(0).getValueType();
    assert(integerType == MVT::i64);
    EVT floatType = op.getValueType();
    assert(!isTypeLegal(integerType));

    SDValue operand = op.getOperand(0);
    SDValue hi;
    SDValue lo;

    if(operand.getOpcode() == ISD::BUILD_PAIR) {
      hi = operand.getOperand(1);
      lo = operand.getOperand(0);
    } else {
      hi = dag.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, operand,
          dag.getIntPtrConstant(1, dl));
      lo = dag.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, operand,
          dag.getIntPtrConstant(0, dl));
    }

    SDValue doubleValue = dag.getNode(ChipsISD::LONG_TO_DOUBLE, dl, MVT::f64,
        hi, lo);
    if(floatType == MVT::f64)
      return doubleValue;
    assert(floatType == MVT::f32);
    return dag.getFPExtendOrRound(doubleValue, dl, MVT::f32);
  }

  SDValue lowerFPToSInt(SDValue op, SelectionDAG& dag) const {
    SDLoc dl(op);
    SDValue operand = op.getOperand(0);
    EVT floatType = operand.getValueType();
    EVT integerType = op.getValueType();
    assert(integerType == MVT::i64);

    SDValue doubleValue = operand;

    if(floatType == MVT::f32)
      doubleValue = dag.getNode(ISD::FP_EXTEND, dl, MVT::f64, operand);

    EVT resultTypes[] = { MVT::i32, MVT::i32 };
    SDValue operands[] = { doubleValue };

    SDValue conversion = dag.getNode(ChipsISD::DOUBLE_TO_LONG, dl,
        resultTypes, operands);
    SDValue hi = conversion.getValue(0);
    SDValue lo = conversion.getValue(1);
    return dag.getNode(ISD::BUILD_PAIR, dl, MVT::i64, lo, hi);
  }

  SDValue lowerLoad(SDValue op, SelectionDAG& dag) const {
    LoadSDNode* node = cast<LoadSDNode>(op.getNode());
    SDLoc dl(op);

//    SDValue const& original_offset = node->getOffset();
//    SDValue offset;
//    if(!original_offset.isUndef())
//      offset = dag.getNode(ISD::SRA, dl, MVT::i32,
//          original_offset, dag.getTargetConstant(2, dl, MVT::i32));
//    else
//      offset = original_offset;

    unsigned alignment = node->getAlignment();
    if(alignment > 4)
      alignment = 4;

    SDValue hi = dag.getLoad(MVT::f32, dl, node->getChain(),
        node->getBasePtr(), node->getPointerInfo(), alignment);
    EVT addrVT = node->getBasePtr().getValueType();
    SDValue loPtr = dag.getNode(ISD::ADD, dl, addrVT,
        node->getBasePtr(), dag.getConstant(4, dl, addrVT));
    SDValue lo = dag.getLoad(MVT::f32, dl, node->getChain(), loPtr,
        node->getPointerInfo(), alignment);

    SDValue subRegHi = dag.getTargetConstant(Chips::DoubleHiIndex, dl, MVT::i32);
    SDValue subRegLo = dag.getTargetConstant(Chips::DoubleLoIndex, dl, MVT::i32);

    SDNode* inDouble = dag.getMachineNode(TargetOpcode::IMPLICIT_DEF,
        dl, MVT::f64);
    inDouble = dag.getMachineNode(TargetOpcode::INSERT_SUBREG, dl,
        MVT::f64, SDValue(inDouble, 0), hi, subRegHi);
    inDouble = dag.getMachineNode(TargetOpcode::INSERT_SUBREG, dl,
        MVT::f64, SDValue(inDouble, 0), lo, subRegLo);

    SDValue outChains[2] = {
      SDValue(hi.getNode(), 1),
      SDValue(lo.getNode(), 1)
    };
    SDValue outChain = dag.getNode(ISD::TokenFactor, dl, MVT::Other, outChains);
    SDValue ops[2] = { SDValue(inDouble, 0), outChain };
    return dag.getMergeValues(ops, dl);
  }

  SDValue lowerStore(SDValue op, SelectionDAG& dag) const {
    StoreSDNode* node = cast<StoreSDNode>(op.getNode());
    SDLoc dl(op);

    assert(node->getValue().getValueType() == MVT::f64);

    SDValue subRegHi = dag.getTargetConstant(Chips::DoubleHiIndex, dl, MVT::i32);
    SDValue subRegLo = dag.getTargetConstant(Chips::DoubleLoIndex, dl, MVT::i32);

    SDNode* hi = dag.getMachineNode(TargetOpcode::EXTRACT_SUBREG,
        dl, MVT::f32, node->getValue(), subRegHi);
    SDNode* lo = dag.getMachineNode(TargetOpcode::EXTRACT_SUBREG,
        dl, MVT::f32, node->getValue(), subRegLo);

    unsigned alignment = node->getAlignment();
    if(alignment > 4)
      alignment = 4;

    SDValue outChains[2];
    outChains[0] = dag.getStore(node->getChain(), dl, SDValue(hi, 0),
        node->getBasePtr(), MachinePointerInfo(), alignment);
    EVT addrVT = node->getBasePtr().getValueType();
    SDValue loPtr = dag.getNode(ISD::ADD, dl, addrVT,
        node->getBasePtr(), dag.getConstant(4, dl, addrVT));
    outChains[1] = dag.getStore(node->getChain(), dl, SDValue(lo, 0),
        loPtr, MachinePointerInfo(), alignment);
    return dag.getNode(ISD::TokenFactor, dl, MVT::Other, outChains);
  }

  SDValue lowerBitcast(SDValue op, SelectionDAG& dag) const {
    SDValue const& operand = op.getOperand(0);
    operand.getNode()->print(errs());
    errs() << "\n";
    //assert(dyn_cast<RegisterSDNode>(operand));
    return operand;
  }

  SDValue lowerBrcond(SDValue op, SelectionDAG& dag) const {
    //errs() << "[CHIPS] lowering brcond\n";

    SDLoc dl(op);
    SDValue chain = op.getOperand(0);
    SDValue condition = op.getOperand(1);
    SDValue target = op.getOperand(2);

    assert(condition.getOpcode() == ISD::SETCC);
    condition = lowerSetcc(condition, dag);

    return dag.getNode(ChipsISD::BRCOND, dl, op.getValueType(),
        chain, condition, target);
    return op;
  }

  SDValue lowerSetcc(SDValue op, SelectionDAG& dag) const {
    MVT type = op.getSimpleValueType();
    assert(!type.isVector());

    SDLoc dl(op);
    SDValue lhs = op.getOperand(0);
    SDValue rhs = op.getOperand(1);
    ISD::CondCode condCode = cast<CondCodeSDNode>(op.getOperand(2))->get();

    //TODO insert condcode
    SDValue boolean = dag.getNode(ChipsISD::CMP, dl, MVT::i32, lhs, rhs);
    return dag.getNode(ChipsISD::SETCC, dl, MVT::i32,
        dag.getConstant(condCode, dl, MVT::i32), boolean); //convert condcode first


//  SDValue EFLAGS = EmitCmp(Op0, Op1, X86CC, dl, DAG);
//  EFLAGS = ConvertCmpIfNecessary(EFLAGS, DAG);
//  return getSETCC(X86CC, EFLAGS, dl, DAG);
//
//  return DAG.getNode(X86ISD::CMP, dl, MVT::i32, Op0, Op1);
//
//static SDValue getSETCC(X86::CondCode Cond, SDValue EFLAGS, const SDLoc &dl,
//                        SelectionDAG &DAG) {
//  return DAG.getNode(X86ISD::SETCC, dl, MVT::i8,
//                     DAG.getConstant(Cond, dl, MVT::i8), EFLAGS);
//}

    return op;
  }

  SDValue lowerAddress(SDValue op, SelectionDAG& dag) const {
    SDLoc dl(op);
    EVT type = op.getValueType();

    SDValue ret;

    if(GlobalAddressSDNode const* globalAddr =
        dyn_cast<GlobalAddressSDNode>(op))
      ret = dag.getTargetGlobalAddress(globalAddr->getGlobal(), dl, type,
          globalAddr->getOffset());
    else if(ConstantPoolSDNode const* constPool = dyn_cast<ConstantPoolSDNode>(op))
      ret = dag.getTargetConstantPool(constPool->getConstVal(), type,
          constPool->getAlignment(), constPool->getOffset());

    return dag.getNode(ChipsISD::ADDR_WRAPPER, dl,
        getPointerTy(dag.getDataLayout()), ret);
    
//    if(BlockAddressSDNode const* blockAddr = dyn_cast<BlockAddressSDNode>(op))
//      return dag.getTargetBlockAddress(blockAddr->getBlockAddress(), type,
//          blockAddr->getOffset());
    llvm_unreachable("cannot lower address");
    //TODO do hi/lo load
    return op;
    //dag.
    return dag.getIntPtrConstant(25, dl);
      //chain = dag.getCopyToReg(chain, dl, loc.getLocReg(), val, glue);
  }

  std::pair<unsigned, TargetRegisterClass const*> getRegForInlineAsmConstraint(
      TargetRegisterInfo const* registerInfo, StringRef constraint,
      MVT type) const {
    if(constraint.size() == 1 && constraint[0] == 'r')
      return std::make_pair(0u, &Chips::AllRegsRegClass);

    return TargetLowering::getRegForInlineAsmConstraint(registerInfo,
        constraint, type);
  }
};

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_HEADER
#define GET_INSTRINFO_MC_DESC
#define GET_INSTRINFO_ENUM
#include "ChipsGenInstrInfo.inc"
class ChipsInstrInfo : public ChipsGenInstrInfo {
private:
  ChipsRegisterInfo registerInfo;

public:
  ChipsInstrInfo() :
    ChipsGenInstrInfo(Chips::ADJCALLSTACKDOWN, Chips::ADJCALLSTACKUP) { }

//    isLoadFromStackSlot — If the specified machine instruction is a direct load from a stack slot, return the register number of the destination and the FrameIndex of the stack slot.
//    isStoreToStackSlot — If the specified machine instruction is a direct store to a stack slot, return the register number of the destination and the FrameIndex of the stack slot.
//    copyPhysReg — Copy values between a pair of physical registers.
//    storeRegToStackSlot — Store a register value to a stack slot.
//    loadRegFromStackSlot — Load a register value from a stack slot.
//    storeRegToAddr — Store a register value to memory.
//    loadRegFromAddr — Load a register value from memory.
//    foldMemoryOperand — Attempt to combine instructions of any load or store instruction for the specified operand(s).
//
  ChipsRegisterInfo const* getRegisterInfo() const {
    return &registerInfo;
  }

private:
  unsigned getDoubleRegCopyInstr(bool isWrite, unsigned doubleReg) const {
    if(isWrite) {
      switch(doubleReg) {
        case Chips::RALo:
          return Chips::MovALoIn;
        case Chips::RAHi:
          return Chips::MovAHiIn;
        case Chips::RBLo:
          return Chips::MovBLoIn;
        case Chips::RBHi:
          return Chips::MovBHiIn;
      }
    } else {
      switch(doubleReg) {
        case Chips::RALo:
          return Chips::MovALoOut;
        case Chips::RAHi:
          return Chips::MovAHiOut;
        case Chips::RBLo:
          return Chips::MovBLoOut;
        case Chips::RBHi:
          return Chips::MovBHiOut;
      }
    }
  }

public:
  void copyPhysReg(MachineBasicBlock& block,
      MachineBasicBlock::iterator instrs,
      DebugLoc const& dl, unsigned destReg, unsigned srcReg,
      bool killSrc) const override {
    bool copyToDoubleReg = Chips::DFpSubRegsRegClass.contains(destReg);
    bool copyFromDoubleReg = Chips::DFpSubRegsRegClass.contains(srcReg);
    if(copyToDoubleReg) {
      unsigned instruction = getDoubleRegCopyInstr(true, destReg);
      BuildMI(block, instrs, dl, get(instruction))
        .addReg(srcReg, getKillRegState(killSrc));
    } else if(copyFromDoubleReg) {
      unsigned instruction = getDoubleRegCopyInstr(false, srcReg);
      BuildMI(block, instrs, dl, get(instruction), destReg);
    } else {
      BuildMI(block, instrs, dl, get(Chips::Addl), destReg)
        .addReg(srcReg, getKillRegState(killSrc))
        .addImm(0);
    }
  }

  void storeRegToStackSlot(MachineBasicBlock& block,
      MachineBasicBlock::iterator instrs,
      unsigned srcReg, bool isKill, int frameIndex,
      TargetRegisterClass const* registerClass,
      TargetRegisterInfo const* registerInfo) const override {
    //llvm_unreachable("foooooooo");
    //TODO refactor
    DebugLoc dl;
    if (instrs != block.end())
      dl = instrs->getDebugLoc();

    MachineFunction* func = block.getParent();
    MachineFrameInfo const& frameInfo = func->getFrameInfo();
    MachineMemOperand* memOperand = func->getMachineMemOperand(
        MachinePointerInfo::getFixedStack(*func, frameIndex),
        MachineMemOperand::MOLoad,
        frameInfo.getObjectSize(frameIndex),
        frameInfo.getObjectAlignment(frameIndex));
//    errs() << "spilling " << frameIndex << ": ";
//    frameInfo.print(*func, errs());
//    errs() << "\n";

    if(registerClass == &Chips::DFpRegsRegClass)
      errs() << "spilling double register\n";

    if(true)//registerClass == &Chips::AllRegsRegClass)
      BuildMI(block, instrs, dl, get(Chips::Store))
        .addFrameIndex(frameIndex)
        .addReg(srcReg, getKillRegState(isKill)).addImm(0).addMemOperand(memOperand);
    else
      llvm_unreachable("Can't spill this register onto stack.");
  }

  void loadRegFromStackSlot(MachineBasicBlock& block,
      MachineBasicBlock::iterator instrs,
      unsigned destReg, int frameIndex, TargetRegisterClass const* registerClass,
      TargetRegisterInfo const* registerInfo) const override {
    //TODO refactor
    DebugLoc dl;
    if (instrs != block.end()) dl = instrs->getDebugLoc();

    MachineFunction* func = block.getParent();
    MachineFrameInfo const& frameInfo = func->getFrameInfo();
    MachineMemOperand* memOperand = func->getMachineMemOperand(
        MachinePointerInfo::getFixedStack(*func, frameIndex),
        MachineMemOperand::MOLoad,
        frameInfo.getObjectSize(frameIndex),
        frameInfo.getObjectAlignment(frameIndex));

//    insertStackSpill(MachineMemOperand::MOLoad, Chips::Load
//    insertStackSpill(MachineMemOperand::MOStore, Chips::Store

    if(true)//registerClass == &Chips::AllRegsRegClass)
      BuildMI(block, instrs, dl, get(Chips::LoadDisp), destReg)
        .addFrameIndex(frameIndex).addImm(0)
        .addMemOperand(memOperand);
    else
      llvm_unreachable("Can't spill this register onto stack.");
  }
  
  unsigned isLoadFromStackSlot(MachineInstr const& instr,
      int& frameIndex) const override {
    if(instr.getOpcode() != Chips::LoadDisp)
      return 0;

    if(instr.getOperand(1).isFI() && instr.getOperand(2).isImm() &&
        instr.getOperand(2).getImm() == 0) {
      frameIndex = instr.getOperand(1).getIndex();
      return instr.getOperand(0).getReg();
    }
    return 0;
  }

  unsigned isStoreToStackSlot(MachineInstr const& instr,
      int& frameIndex) const override {
    if(instr.getOpcode() != Chips::Store)
      return 0;

    if(instr.getOperand(0).isFI() && instr.getOperand(2).isImm() &&
        instr.getOperand(2).getImm() == 0) {
      frameIndex = instr.getOperand(0).getIndex();
      return instr.getOperand(1).getReg();
    }
    return 0;
  }

  static void parseCondBranch(MachineInstr* lastInstr,
      MachineBasicBlock*& target,
      SmallVectorImpl<MachineOperand>& conditions) {
    conditions.push_back(MachineOperand::CreateReg(
          lastInstr->getOperand(0).getReg(), /*isDef=*/false));
    target = lastInstr->getOperand(1).getMBB();
  }

  static bool isUncondBranchOpcode(unsigned opcode) {
    return opcode == Chips::Goto;
  }
  static bool isCondBranchOpcode(unsigned opcode) {
    return opcode == Chips::JmpIfTrue;
  }

  bool analyzeBranch(MachineBasicBlock& block, 
      MachineBasicBlock*& trueBlock, MachineBasicBlock*& falseBlock,
      SmallVectorImpl<MachineOperand>& conditions,
      bool allowModify) const override {
    MachineBasicBlock::iterator instrs = block.getLastNonDebugInstr();
    if(instrs == block.end())
      return false;

    if(!isUnpredicatedTerminator(*instrs))
      return false;

    // Get the last instruction in the block.
    MachineInstr* lastInstr = &*instrs;
    unsigned lastOpcode = lastInstr->getOpcode();

    // If there is only one terminator instruction, process it.
    if(instrs == block.begin() || !isUnpredicatedTerminator(*--instrs)) {
      if(isUncondBranchOpcode(lastOpcode)) {
        trueBlock = lastInstr->getOperand(0).getMBB();
        return false;
      }
      if(isCondBranchOpcode(lastOpcode)) {
        // Block ends with fall-through condbranch.
        parseCondBranch(lastInstr, trueBlock, conditions);
        return false;
      }
      return true; // Can't handle indirect branch.
    }

    // Get the instruction before it if it is a terminator.
    MachineInstr* secondLastInstr = &*instrs;
    unsigned secondLastOpcode = secondLastInstr->getOpcode();

    //TODO refactor!

    // If AllowModify is true and the block ends with two or more unconditional
    // branches, delete all but the first unconditional branch.
    if(allowModify && isUncondBranchOpcode(lastOpcode)) {
      while(isUncondBranchOpcode(secondLastOpcode)) {
        lastInstr->eraseFromParent();
        lastInstr = secondLastInstr;
        lastOpcode = lastInstr->getOpcode();
        if(instrs == block.begin() || !isUnpredicatedTerminator(*--instrs)) {
          // Return now the only terminator is an unconditional branch.
          trueBlock = lastInstr->getOperand(0).getMBB();
          return false;
        } else {
          secondLastInstr = &*instrs;
          secondLastOpcode = secondLastInstr->getOpcode();
        }
      }
    }


    //TODO handling more than 2 unconditional branches not helpful if
    //allowModify is false?

    // If there are three terminators, we don't know what sort of block this is.
    if(secondLastInstr && instrs != block.begin() &&
        isUnpredicatedTerminator(*--instrs))
      return true;

    // If the block ends with a B and a Bcc, handle it.
    if(isCondBranchOpcode(secondLastOpcode) &&
        isUncondBranchOpcode(lastOpcode)) {
      parseCondBranch(secondLastInstr, trueBlock, conditions);
      falseBlock = lastInstr->getOperand(0).getMBB();
      return false;
    }

    // If the block ends with two unconditional branches, handle it.  The second
    // one is not executed.
    if(isUncondBranchOpcode(secondLastOpcode) && 
        isUncondBranchOpcode(lastOpcode)) {
      trueBlock = secondLastInstr->getOperand(0).getMBB();
      return false;
    }

    // ...likewise if it ends with an indirect branch followed by an unconditional
    // branch.
//    if (isIndirectBranchOpcode(SecondLastOpc) && isUncondBranchOpcode(LastOpc)) {
//      I = LastInst;
//      if (AllowModify)
//        I->eraseFromParent();
//      return true;
//    }

    // Otherwise, can't handle this.
    return true;
  }

  unsigned insertBranch(MachineBasicBlock& block,
      MachineBasicBlock* trueBlock, MachineBasicBlock* falseBlock, 
      ArrayRef<MachineOperand> conditions, DebugLoc const& dl, 
      int* bytesAdded) const override {
    //errs() << "inserting branch\n";
    assert(trueBlock && "insertBranch must not be told to insert a fallthrough");
    assert((conditions.size() == 1 || conditions.size() == 0) &&
          "Chips branch conditions should have one component!");
    assert(!bytesAdded && "code size not handled");

    if(conditions.empty()) {
      assert(!falseBlock && "Unconditional branch with multiple successors!");
      BuildMI(&block, dl, get(Chips::Goto)).addMBB(trueBlock);
      return 1;
    }

    // Conditional branch
    unsigned booleanRegister = conditions[0].getReg();

    //if(IsIntegerCC(CC))
    BuildMI(&block, dl, get(Chips::JmpIfTrue))
      .addReg(booleanRegister)
      .addMBB(trueBlock);
    //TODO handle floating point
//    else
//      BuildMI(&MBB, DL, get(SP::FBCOND)).addMBB(TBB).addImm(CC);
    if(!falseBlock)
      return 1;

    BuildMI(&block, dl, get(Chips::Goto)).addMBB(falseBlock);
    return 2;
  }

  unsigned removeBranch(MachineBasicBlock& block,
      int* bytesRemoved) const override {
    //errs() << "removing branch\n";
    assert(!bytesRemoved && "code size not handled");

    MachineBasicBlock::iterator instrs = block.end();
    unsigned i = 0;
    while(instrs != block.begin()) {
      --instrs;

      if(instrs->isDebugValue())
        continue;

      if(instrs->getOpcode() != Chips::Goto &&
          instrs->getOpcode() != Chips::JmpIfTrue)
        break; // Not a branch

      instrs->eraseFromParent();
      instrs = block.end();
      i += 1;
    }
    return i;
  }

  bool shouldScheduleLoadsNear(SDNode* load1, SDNode* load2,
      int64_t offset1, int64_t offset2, unsigned numLoads) const override {
    return true;
  }

  ScheduleHazardRecognizer* CreateTargetHazardRecognizer(
      TargetSubtargetInfo const* subtargetInfo,
      ScheduleDAG const* dag) const override;
  
  ScheduleHazardRecognizer* CreateTargetPostRAHazardRecognizer(
      InstrItineraryData const* itineraryData,
      ScheduleDAG const* dag) const override;

  ScheduleHazardRecognizer* CreateTargetMIHazardRecognizer(
      InstrItineraryData const* itineraryData,
      ScheduleDAG const* dag) const override;
};

class ChipsHazardRecognizer : public ScoreboardHazardRecognizer {
private:
  MachineInstr* lastInstr;
  unsigned issueCount;
  TargetInstrInfo const* instrInfo;

public:
  ChipsHazardRecognizer(InstrItineraryData const* itineraryData,
      ScheduleDAG const* dag, TargetInstrInfo const* instrInfo) : 
    ScoreboardHazardRecognizer(itineraryData, dag, "post-re-sched"),
    lastInstr(nullptr),
    instrInfo(instrInfo) { }

  void Reset() override {
    lastInstr = nullptr;
    issueCount = 0;
  }

//  unsigned PreEmitNoops(SUnit* su) override {
//    errs() << "preemitnoops called\n";
//    if(atIssueLimit()) {
//      errs() << "at issue limig\n";
//      return 1;
//    }
//    return 0;
//  }

//  ScheduleHazardRecognizer::HazardType getHazardType(SUnit* su, int stalls) {
////    assert(stalls == 0);
////
////    MachineInstr* instr = su->getInstr();
////
////    if(!instr->isDebugValue()) {
////      //TODO detect RAW hazard and emit nop
////    }
////
//    auto ret = ScoreboardHazardRecognizer::getHazardType(su, stalls);
//    if(ret == Hazard) {
//      errs() << "returning noop\n";
//      return NoopHazard;
//    }
//    return ret;
//  }

  void AdvanceCycle() override {
    if(issueCount == 0) {
      errs() << "stall detected\n";
      lastInstr->print(errs());

      MachineBasicBlock* basicBlock = lastInstr->getParent();
      MachineFunction* func = basicBlock->getParent();
//      MachineInstr* newInstr = func->CreateMachineInstr(
//          instrInfo->get(Chips::Nop), lastInstr->getDebugLoc());
      for(auto instrs = basicBlock->begin();
          instrs != basicBlock->end(); ++instrs) {
        if(&*instrs == lastInstr) {
//          basicBlock->insertAfter(instrs, newInstr);
          BuildMI(*basicBlock, instrs, lastInstr->getDebugLoc(),
              instrInfo->get(Chips::Nop));
        }
        else {
          errs() << "tick tick ";
          instrs->print(errs());
        }
      }
//      MF->DeleteMachineInstr(NewMI);
    }
    issueCount = 0;

    ScoreboardHazardRecognizer::AdvanceCycle();

  }
//  void EmitNoop() override {
//    errs() << "emitNoop called\n";
//    ScoreboardHazardRecognizer::EmitNoop();
//  }

  void EmitInstruction(SUnit* su) override {
    MachineInstr* instr = su->getInstr();
    if(!instr->isDebugValue())
      lastInstr = instr;

    ++issueCount;
    ScoreboardHazardRecognizer::EmitInstruction(su);
  }
};

void ChipsRegisterInfo::eliminateFrameIndex(
    MachineBasicBlock::iterator instrs, int SPAdj, unsigned fiOperandNum,
    RegScavenger* RS) const {
  //errs() << "[CHIPS] eliminating\n";
  MachineInstr& instr = *instrs;
  DebugLoc dl = instr.getDebugLoc();
  MachineFunction& func = *instr.getParent()->getParent();
  MachineRegisterInfo& regInfo = func.getRegInfo();
  TargetInstrInfo const& instrInfo = *func.getSubtarget().getInstrInfo();

  auto& fiOperand = instr.getOperand(fiOperandNum);

  int frameIndex = fiOperand.getIndex();
  unsigned frameReg;
  int offset = getFrameLowering(func)->
    getFrameIndexReference(func, frameIndex, frameReg);
  offset = scaledOffset(offset);
  MachineBasicBlock& block = *instr.getParent();

//  unsigned scratchReg = regInfo.createVirtualRegister(&Chips::RegsRegClass);
//  errs() << "[CHIPS] Choosing " << scratchReg << " as virt reg\n";

  //TODO somehow choose free register (create virtual?)
  //     use regscavenger
  unsigned scratchReg = Chips::R12;

  if(isInt<13>(offset)) {
    int displacementOperand = -1;
    switch(instr.getOpcode()) {
      case Chips::LoadDisp:
      case Chips::Load8Disp:
      case Chips::Load16Disp:
        displacementOperand = 1;
    }
    switch(instr.getOpcode()) {
      case Chips::Store:
      case Chips::Store8:
      case Chips::Store16:
        displacementOperand = 2;
    }

    if(displacementOperand != -1) {
      fiOperand.ChangeToRegister(frameReg, false, false);
      instr.getOperand(fiOperandNum + displacementOperand).setImm(offset);
      return;
    }
  }
//  if(instr.getOpcode() == Chips::LoadDisp && isInt<14>(offset)) {
//    //errs() << "have load from stack\n";
//    fiOperand.ChangeToRegister(frameReg, false, false);
//    instr.getOperand(fiOperandNum + 1).setImm(offset);
//
////    MI.setDesc(TII.get(SP::LDDFri));
////    MI.getOperand(0).setReg(DestOddReg);
////    Offset += 8;
//    return;
//  }
//  if(instr.getOpcode() == Chips::Store && isInt<14>(offset)) {
//    fiOperand.ChangeToRegister(frameReg, false, false);
//    instr.getOperand(fiOperandNum + 2).setImm(offset);
//    return;
//  }

  BuildMI(block, instrs, dl, instrInfo.get(Chips::Addl), scratchReg)
      .addReg(frameReg)
      .addImm(offset);
      //.addReg(ScratchReg, RegState::Kill);

  fiOperand.ChangeToRegister(scratchReg, false, false);
}

#define GET_SUBTARGETINFO_HEADER
#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_MC_DESC
#include "ChipsGenSubtargetInfo.inc"

class ChipsSubtarget : public ChipsGenSubtargetInfo {
private:
  ChipsTargetLowering lowering;
  ChipsFrameLowering frameLowering;
  ChipsInstrInfo instrInfo;
  InstrItineraryData itineraryData;

public:
  ChipsSubtarget(Triple const& triple, std::string const& cpu,
      std::string const& fs, TargetMachine const& tm) :
    ChipsGenSubtargetInfo(triple, cpu, fs),
    lowering(tm, *this),
    frameLowering(*this) {
    itineraryData = getInstrItineraryForCPU("chips");
  }

  ChipsTargetLowering const* getTargetLowering() const override {
    return &lowering;
  }

  TargetFrameLowering const* getFrameLowering() const override {
    return &frameLowering;
  }

  ChipsInstrInfo const* getInstrInfo() const override {
    return &instrInfo;
  }

  ChipsRegisterInfo const* getRegisterInfo() const override {
    return instrInfo.getRegisterInfo();
  }
};

ScheduleHazardRecognizer* ChipsInstrInfo::CreateTargetHazardRecognizer(
    TargetSubtargetInfo const* subtargetInfo,
    ScheduleDAG const* dag) const {
  if(usePreRAHazardRecognizer()) {
    InstrItineraryData const* itineraryData = static_cast<
      ChipsSubtarget const*>(subtargetInfo)->getInstrItineraryData();
    return new ScoreboardHazardRecognizer(itineraryData, dag, "pre-RA-sched");
  }
  return TargetInstrInfo::CreateTargetHazardRecognizer(subtargetInfo, dag);
}

ScheduleHazardRecognizer* ChipsInstrInfo::CreateTargetPostRAHazardRecognizer(
    InstrItineraryData const* itineraryData,
    ScheduleDAG const* dag) const {
//  if(subtarget.isThumb2() || Subtarget.hasVFP2())
//    return (ScheduleHazardRecognizer *)new ARMHazardRecognizer(II, DAG);
//  return TargetInstrInfo::CreateTargetPostRAHazardRecognizer(itineraryData, dag);

//  return new ChipsHazardRecognizer(itineraryData, dag, this);
  return new ScoreboardHazardRecognizer(itineraryData, dag, "mi-sched");
//  return TargetInstrInfo::CreateTargetPostRAHazardRecognizer(itineraryData,
//      dag);
}

ScheduleHazardRecognizer* ChipsInstrInfo::CreateTargetMIHazardRecognizer(
    InstrItineraryData const* itineraryData,
    ScheduleDAG const* dag) const {
  return new ScoreboardHazardRecognizer(itineraryData, dag, "mi-sched");
  //return new ChipsHazardRecognizer(itineraryData, dag, this);
}


SDValue ChipsTargetLowering::LowerFormalArguments(SDValue chain,
    CallingConv::ID conv, bool isVarArg, 
    SmallVectorImpl<ISD::InputArg> const& ins, SDLoc const& dl,
    SelectionDAG& dag, SmallVectorImpl<SDValue>& inVals) const {
  MachineFunction& func = dag.getMachineFunction();
  MachineRegisterInfo& regInfo = func.getRegInfo();
  //SparcMachineFunctionInfo *FuncInfo = func.getInfo<ChipsMachineFunctionInfo>();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> argLocs;
  CCState ccInfo(conv, isVarArg, func, argLocs, *dag.getContext());
  ccInfo.AnalyzeFormalArguments(ins, CC_Chips);

  unsigned InIdx = 0;
  for(unsigned i = 0, e = argLocs.size(); i != e; ++i, ++InIdx) {
    CCValAssign& argLoc = argLocs[i];

    if(argLoc.isRegLoc()) {
      unsigned virtualRegister = regInfo.createVirtualRegister(
          &Chips::IntRegsRegClass);
      func.getRegInfo().addLiveIn(argLoc.getLocReg(), virtualRegister);
      SDValue argument = dag.getCopyFromReg(chain, dl, virtualRegister,
          MVT::i32);
      if(argLoc.getLocVT() == MVT::f32)
        argument = dag.getNode(ISD::BITCAST, dl, MVT::f32, argument);
      else {
        //already extended in LowerCall
        switch(argLoc.getLocInfo()) {
        case CCValAssign::SExt:
          argument = dag.getNode(ISD::AssertSext, dl, argLoc.getLocVT(),
              argument, dag.getValueType(argLoc.getValVT()));
          break;
        case CCValAssign::ZExt:
          argument = dag.getNode(ISD::AssertZext, dl, argLoc.getLocVT(),
              argument, dag.getValueType(argLoc.getValVT()));
          break;
        default:
          break;
        }
//        argument = dag.getNode(ISD::AssertSext, dl, MVT::i32, argument,
//                          dag.getValueType(argLoc.getLocVT()));
//        argument = dag.getNode(ISD::TRUNCATE, dl, argLoc.getLocVT(), argument);
      }
      inVals.push_back(argument);
      continue;
    }

    assert(argLoc.isMemLoc());

    unsigned offset = argLoc.getLocMemOffset();
    //offset >>= 2;
    auto pointerType = getPointerTy(dag.getDataLayout());

    //errs() << "[CHIPS] offset: " << offset << "\n";

    int frameIndex = func.getFrameInfo().CreateFixedObject(4, offset, true); //TODO refactor 4
    SDValue fiNode = dag.getFrameIndex(frameIndex, pointerType);
    SDValue Load;
    //TODO refactor: remove if
    if(argLoc.getValVT() == MVT::i32 || argLoc.getValVT() == MVT::f32 ||
        argLoc.getValVT() == MVT::f64) {
      Load = dag.getLoad(argLoc.getValVT(), dl, chain, fiNode,
          MachinePointerInfo::getFixedStack(func, frameIndex));
    } else {
      // We shouldn't see any other value types here.
      llvm_unreachable("Unexpected ValVT encountered in frame lowering.");
    }
    inVals.push_back(Load);
  }

//    if (MF.getFunction().hasStructRetAttr()) {
//      // Copy the SRet Argument to SRetReturnReg.
//      SparcMachineFunctionInfo *SFI = MF.getInfo<SparcMachineFunctionInfo>();
//      unsigned Reg = SFI->getSRetReturnReg();
//      if (!Reg) {
//        Reg = MF.getRegInfo().createVirtualRegister(&SP::IntRegsRegClass);
//        SFI->setSRetReturnReg(Reg);
//      }
//      SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), dl, Reg, InVals[0]);
//      Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, Copy, Chain);
//    }

  return chain;
}

int llvm::ChipsFrameLowering::getFrameIndexReference(
    MachineFunction const& func, int frameIndex,
    unsigned& frameReg) const {
  //errs() << "[CHIPS] getFrameIndexReference\n";
  ChipsSubtarget const& subtarget = func.getSubtarget<ChipsSubtarget>();
  MachineFrameInfo const& frameInfo = func.getFrameInfo();
  TargetRegisterInfo const& registerInfo = *subtarget.getRegisterInfo();

  //TODO check this?
  //} else if (RegInfo->needsStackRealignment(MF)) {

  //frameReg = registerInfo.getFrameRegister(func);
  frameReg = Chips::R3; //TODO refactor

  //errs() << "framereg is " << frameReg << " " << Chips::r4 << "\n";
  int64_t offset = frameInfo.getObjectOffset(frameIndex);
  offset -= roundedToAlignment(frameInfo.getStackSize());
  //offset += frameInfo.getStackSize();
//  errs() << "[CHIPS] offset: " << offset << ", stack size: " <<
//    frameInfo.getStackSize() << ", index: " << frameIndex << "\n";
  return offset;
}

void llvm::ChipsFrameLowering::emitPrologue(MachineFunction& func,
    MachineBasicBlock& block) const {
  //skip callee-saved register stores?
  MachineFrameInfo& frameInfo = func.getFrameInfo();
  int64_t stackSize = frameInfo.getStackSize();
  stackSize = scaledOffset(stackSize);
  auto instrs = block.begin();
  DebugLoc dl;

  ChipsRegisterInfo const& registerInfo = *((ChipsRegisterInfo*)
      subtarget.getRegisterInfo());

  bool needsStackRealignment = registerInfo.needsStackRealignment(func);

  if(!needsStackRealignment && frameInfo.getMaxAlignment() > getStackAlignment())
    report_fatal_error("stack realignment not possible");

  if(frameInfo.getMaxAlignment() > 0)
    stackSize = alignTo(stackSize, frameInfo.getMaxAlignment());

  //TODO refactor
  stackSize = roundedToAlignment(stackSize);
  frameInfo.setStackSize(stackSize);

  //TODO put this into a MachineFunctionPass (add with
  //ChipsPassConfig::addPostRegAlloc?) or maybe set
  //TargetLoweringObjectFile::StackSizesSection
  int64_t maxStackSize = stackSize;
//  if(needsStackRealignment)
//    maxStackSize += frameInfo.getMaxAlignment() - 1;
  stackSizesOut << func.getName().str() << " " << maxStackSize << "\n";
  stackSizesOut.flush();

  if(stackSize == 0)
    return;

  //TODO add flag FrameSetup (see RISCV)?
  BuildMI(block, instrs, dl, subtarget.getInstrInfo()->get(Chips::Addl),
      Chips::R3)
    .addReg(Chips::R3)
    .addImm(stackSize);

//  if(needsStackRealignment)
}
void llvm::ChipsFrameLowering::emitEpilogue(MachineFunction& func,
    MachineBasicBlock& block) const {
  MachineFrameInfo const& frameInfo = func.getFrameInfo();
  int64_t stackSize = frameInfo.getStackSize();
  auto instrs = block.getLastNonDebugInstr();
  DebugLoc dl(instrs->getDebugLoc());

  if(stackSize == 0)
    return;

  BuildMI(block, instrs, dl, subtarget.getInstrInfo()->get(Chips::Addl),
      Chips::R3)
    .addReg(Chips::R3)
    .addImm(-stackSize);
}

ChipsTargetLowering::ChipsTargetLowering(TargetMachine const& tm,
    ChipsSubtarget const& subtarget) :
  TargetLowering(tm), subtarget(subtarget) {
  MVT pointerType = MVT::getIntegerVT(tm.getPointerSize() * 8);
//  MVT pointerType = MVT::getIntegerVT(tm.getPointerSize(0) * 8);

  addRegisterClass(MVT::i32, &Chips::IntRegsRegClass);
  addRegisterClass(MVT::i32, &Chips::AllRegsRegClass);
  addRegisterClass(MVT::f32, &Chips::AllRegsRegClass);
  addRegisterClass(MVT::f32, &Chips::FpRegsRegClass);
//  addRegisterClass(MVT::f64, &Chips::DFpRegsRegClass);
  //addRegisterClass(MVT::i64, &Chips::Int64RegsRegClass);
  //addRegisterClass(MVT::i64, &Chips::DFpRegsRegClass);
  addRegisterClass(MVT::f64, &Chips::DFpRegsRegClass);
  setStackPointerRegisterToSaveRestore(Chips::R3);

  setOperationAction(ISD::LOAD, MVT::f64, Custom);
  setOperationAction(ISD::STORE, MVT::f64, Custom);

  setOperationAction(ISD::SINT_TO_FP, MVT::i64, Custom);
  setOperationAction(ISD::FP_TO_SINT, MVT::i64, Custom);

  setOperationAction(ISD::FNEG, MVT::f32, Expand);
  setOperationAction(ISD::UINT_TO_FP, MVT::i32, Expand);
  setOperationAction(ISD::FP_TO_UINT, MVT::i32, Expand);
//  for(auto condCode : { 
//      ISD::SETOLT, 
//      ISD::SETO,
//      ISD::SETOEQ,
//      ISD::SETOGT })
//    setCondCodeAction(condCode, MVT::f32, LibCall);
//  setOperationAction(ISD::SETCC, MVT::f32, LibCall);

//  setOperationAction(ISD::SETCC, MVT::i32, LibCall);
//  setLibcallName(RTLIB::UDIVREM_I64, "__udivmoddi4");
//  setLibcallName(RTLIB::UDIVREM_I128, "__udivmodti4");
//
//  // Several of the runtime library functions use a special calling conv
//  setLibcallCallingConv(RTLIB::SDIVREM_I8, CallingConv::AVR_BUILTIN);
//  setLibcallCallingConv(RTLIB::SDIVREM_I16, CallingConv::AVR_BUILTIN);
//  setLibcallCallingConv(RTLIB::UDIVREM_I8, CallingConv::AVR_BUILTIN);
//  setLibcallCallingConv(RTLIB::UDIVREM_I16, CallingConv::AVR_BUILTIN);
//
//  // Trigonometric rtlib functions
//  setLibcallName(RTLIB::SIN_F32, "sin");
//  setLibcallName(RTLIB::COS_F32, "cos");


  //setOperationAction(ISD::BRCOND, MVT::Other, Custom);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::BR_CC, MVT::f32, Expand);
  setOperationAction(ISD::BR_CC, MVT::f64, Expand);

//  setOperationAction(ISD::BITCAST, MVT::f32, Custom);
//  setOperationAction(ISD::BITCAST, MVT::i32, Custom);

  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::f32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::f64, Expand);
  //TODO implement select
//  setOperationAction(ISD::SELECT, MVT::i32, Expand);
//  setOperationAction(ISD::SELECT, MVT::f32, Expand);
//  setOperationAction(ISD::SELECT, MVT::f64, Expand);

//  setOperationAction(ISD::BRCOND           , MVT::Other, Custom);
//  for (auto VT : { MVT::f32, MVT::f64, MVT::f80, MVT::f128,
//                   MVT::i8,  MVT::i16, MVT::i32, MVT::i64 }) {
//    setOperationAction(ISD::BR_CC,     VT, Expand);
//    setOperationAction(ISD::SELECT_CC, VT, Expand);
//  }

  for(MVT vt : MVT::fp_valuetypes()) {
    setLoadExtAction(ISD::EXTLOAD, vt, MVT::f32, Expand);
    setLoadExtAction(ISD::EXTLOAD, vt, MVT::f64, Expand);
  }

  //setOperationAction(ISD::FP_TO_SINT, MVT::i32, Promote);
  //setOperationPromotedToType(ISD::FP_TO_SINT, MVT::i32, MVT::i64);
  //TODO need regclass for i64 for that
  //setOperationAction(ISD::FP_TO_SINT, MVT::i64, Legal);

  setOperationAction(ISD::GlobalAddress, pointerType, Custom);
  setOperationAction(ISD::ConstantPool, pointerType, Custom);
  setOperationAction(ISD::BlockAddress, pointerType, Custom);

  setMinimumJumpTableEntries(INT_MAX);

//  for(auto vt : { MVT::i8, MVT::i16 }) {
//    setOperationAction(ISD::LOAD, vt, Promote);
//    setOperationAction(ISD::STORE, vt, Promote);
//  }

  setTruncStoreAction(MVT::f64, MVT::f32, Expand);
  //setTruncStoreAction(MVT::i32, MVT::i8, Expand);
  //setTruncStoreAction(MVT::i32, MVT::i1, Expand);
//  for(auto ext : { ISD::EXTLOAD, ISD::SEXTLOAD, ISD::ZEXTLOAD })
//    setLoadExtAction(ext, MVT::i32, MVT::i16, Promote);

//  for(auto ext : { ISD::EXTLOAD, ISD::SEXTLOAD, ISD::ZEXTLOAD }) {
//    for(MVT vt : { MVT::i8, MVT::i16 })
//      setLoadExtAction(ext, MVT::i32, vt, Legal);
//  }
  for(auto vt : { MVT::i8, MVT::i16 })
    setOperationAction(ISD::SIGN_EXTEND_INREG, vt, Expand);
  for(auto ext : { ISD::EXTLOAD, ISD::SEXTLOAD, ISD::ZEXTLOAD })
    setLoadExtAction(ext, MVT::i32, MVT::i1, Promote);

  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::MULHS, MVT::i32, Expand);
//  setOperationAction(ISD::MULHU, MVT::i32, Expand);

//  setCondCodeAction(ISD::SETULT, MVT::i32, Expand);

  //setOperationAction(ISD::LOAD, MVT::f64, Expand);
  //setOperationAction(ISD::STORE, MVT::f64, Promote);
  //setOperationPromotedToType(ISD::STORE, MVT::f64, MVT::i64);

  computeRegisterProperties(subtarget.getRegisterInfo());
}


SDValue ChipsTargetLowering::LowerCall(
    TargetLowering::CallLoweringInfo& loweringInfo,
    SmallVectorImpl<SDValue>& inVals) const {
  SelectionDAG &dag                     = loweringInfo.DAG;
  SDLoc &dl                             = loweringInfo.DL;
  SmallVectorImpl<ISD::OutputArg> &outs = loweringInfo.Outs;
  SmallVectorImpl<SDValue> &outVals     = loweringInfo.OutVals;
  SmallVectorImpl<ISD::InputArg> &ins   = loweringInfo.Ins;
  SDValue chain                         = loweringInfo.Chain;
  SDValue callee                        = loweringInfo.Callee;
  bool& isTailCall                      = loweringInfo.IsTailCall;
  CallingConv::ID callConv              = loweringInfo.CallConv;
  bool isVarArg                         = loweringInfo.IsVarArg;

  isTailCall = false;

  SmallVector<CCValAssign, 16> argLocs;
  CCState ccInfo(callConv, isVarArg, dag.getMachineFunction(), argLocs,
      *dag.getContext());
  ccInfo.AnalyzeCallOperands(outs, CC_Chips);

  unsigned argsSize = roundedToAlignment(ccInfo.getNextStackOffset());

  MachineFrameInfo& frameInfo = dag.getMachineFunction().getFrameInfo();

  SmallVector<SDValue, 8> byValArgs;
  createByvalArgCopies(dag, frameInfo, outs, outVals, chain, dl, byValArgs);

  chain = dag.getCALLSEQ_START(chain, argsSize, 0, dl);

  SmallVector<std::pair<unsigned, SDValue>, 8> regsToPass;
  SmallVector<SDValue, 8> memOpChains;

  bool hasStructRetAttr = false;
  for(unsigned i = 0, realArgI = 0, byValArgI = 0, e = argLocs.size();
      i != e; ++i) {
    CCValAssign& argLoc = argLocs[i];
    ISD::ArgFlagsTy flags = outs[i].Flags;

    SDValue arg = flags.isByVal() ? byValArgs[byValArgI++] : outVals[i];
    if(!arg)
      continue;

    arg = extendOutgoingArgIfNecessary(dag, dl, argLoc, arg);

    if(argLoc.needsCustom()) {
      //...
    }

    if(argLoc.isRegLoc()) {
      if(argLoc.getLocVT() == MVT::f32)
        arg = dag.getNode(ISD::BITCAST, dl, MVT::i32, arg);
      regsToPass.push_back(std::make_pair(argLoc.getLocReg(), arg));
      continue;
    }

    assert(argLoc.isMemLoc());
    //TODO replace Chips::r4 with FrameReg

    // Arg is to be passed on the stack. To do this, make a store.
    SDValue stackPtr = dag.getRegister(Chips::R3, MVT::i32); //TODO replace with StackPtr
    unsigned offset = argLoc.getLocMemOffset();
    offset = scaledOffset(offset);
    SDValue offset_node = dag.getIntPtrConstant(offset, dl);
    //TODO use addl here (or pattern match add with constant in td)
    offset_node = dag.getNode(ISD::ADD, dl, MVT::i32, stackPtr, offset_node);
    memOpChains.push_back(
        dag.getStore(chain, dl, arg, offset_node, MachinePointerInfo()));
  }

  // Emit all stores, make sure the occur before any copies into physregs.
  if(!memOpChains.empty())
    chain = dag.getNode(ISD::TokenFactor, dl, MVT::Other, memOpChains);

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emitted instructions must be
  // stuck together.
  // TODO move into upper loop? -> no because chain would become unordered
  SDValue inFlag;
  for(unsigned i = 0, e = regsToPass.size(); i != e; ++i) {
    unsigned reg = regsToPass[i].first;
    chain = dag.getCopyToReg(chain, dl, reg, regsToPass[i].second, inFlag);
    inFlag = chain.getValue(1);
  }

  //TODO own func makeGlobalAddressTarget
  if(GlobalAddressSDNode* global = dyn_cast<GlobalAddressSDNode>(callee))
    callee = dag.getTargetGlobalAddress(global->getGlobal(), dl, MVT::i32,
        0, 0);
  else if(ExternalSymbolSDNode* sym = dyn_cast<ExternalSymbolSDNode>(callee))
    callee = dag.getTargetExternalSymbol(sym->getSymbol(), MVT::i32, 0);

  // Returns a chain & a flag for retval copy to use
  SDVTList nodeTypes = dag.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> ops;
  ops.push_back(chain);
  ops.push_back(callee);
  for(unsigned i = 0, e = regsToPass.size(); i != e; ++i)
    ops.push_back(dag.getRegister(regsToPass[i].first,
          regsToPass[i].second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  ChipsRegisterInfo const& registerInfo = *subtarget.getRegisterInfo();
  uint32_t const* mask = registerInfo.getCallPreservedMask(
      dag.getMachineFunction(), callConv);
  assert(mask && "Missing call preserved mask for calling convention");
  ops.push_back(dag.getRegisterMask(mask));

  if(inFlag.getNode())
    ops.push_back(inFlag);

  chain = dag.getNode(ChipsISD::CALL, dl, nodeTypes, ops);
  inFlag = chain.getValue(1);

  chain = dag.getCALLSEQ_END(chain, dag.getIntPtrConstant(argsSize, dl, true),
      dag.getIntPtrConstant(0, dl, true), inFlag, dl);
  inFlag = chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> rvLocs;
  CCState rvInfo(callConv, isVarArg, dag.getMachineFunction(), rvLocs,
      *dag.getContext());

  rvInfo.AnalyzeCallResult(ins, RetCC_Chips);

  // Copy all of the result registers out of their specified physreg.
  for(unsigned i = 0; i != rvLocs.size(); ++i) {
    CCValAssign& rvLoc = rvLocs[i];
    chain = dag.getCopyFromReg(chain, dl, rvLoc.getLocReg(),
        rvLoc.getValVT(), inFlag).getValue(1);
    inFlag = chain.getValue(2);
    inVals.push_back(chain.getValue(0));
  }

  return chain;
}

class ChipsTargetMachine : public LLVMTargetMachine {
private:
  ChipsELFTargetObjectFile loweringObjectFile;
  ChipsSubtarget subtarget;

public:
  ChipsTargetMachine(Target const& target, Triple const& triple, StringRef cpu, 
      StringRef fs, TargetOptions const& options, 
      Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM, 
      CodeGenOpt::Level OL, bool jit) :
    LLVMTargetMachine(
        target, "E-p:32:32:32-n32-i8:8:8-a:0:32-f32:32:32-S32",
        triple, cpu, fs, options, Reloc::Static,
        CodeModel::Small, OL),
    subtarget(triple, cpu, fs, *this) {
      initAsmInfo();
    }

  TargetPassConfig* createPassConfig(PassManagerBase& PM) override;
  TargetLoweringObjectFile* getObjFileLowering() const override {
    return (TargetLoweringObjectFile*)&loweringObjectFile;
  }

  ChipsSubtarget const* getSubtargetImpl(Function const& F) const override {
    return &subtarget;
  }
};

FunctionPass* createInstSelectorPass(ChipsTargetMachine& tm);

namespace {
  class MachineOperandInfo {
  private:
    unsigned stageToForwardFrom;

  public:
    MachineOperandInfo(unsigned stageToForwardFrom) :
        stageToForwardFrom(stageToForwardFrom) { }

    bool mustBeForwarded() const {
      return stageToForwardFrom > 0;
    }
    unsigned getStageToForwardFrom() const {
      return stageToForwardFrom;
    }
  };

  class MachineFuncInfo : public MachineFunctionInfo {
  private:
    std::map<MachineInstr const*, std::vector<MachineOperandInfo>> operandInfos;

  public:
    MachineFuncInfo(MachineFunction&) { }

    std::vector<MachineOperandInfo>& getOperandInfos(
        MachineInstr const* instr) {
      return operandInfos[instr];
    }
    std::vector<MachineOperandInfo> const& getOperandInfos(
        MachineInstr const* instr) const {
      auto ret = operandInfos.find(instr);
      assert(ret != operandInfos.end());
      return ret->second;
    }
  };

  class ChipsScheduler : public MachineFunctionPass {
  public:
    ChipsScheduler() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction& func) override {
//      errs() << "running on " << func.getName() << "\n"; 

      for(auto& block : func)
        runOnBasicBlock(*func.getInfo<MachineFuncInfo>(), block);

      bool modified = true;
      return modified;
    }

    void runOnBasicBlock(MachineFuncInfo& funcInfo, MachineBasicBlock& block) {

      for(auto& instr : block) {
        for(auto operands = instr.operands_begin(), end = instr.operands_end();
            operands != end; ++operands)
          funcInfo.getOperandInfos(&instr).emplace_back(1);
      }


      //writeDelay(instr) = number of cycles it takes for the result of
      //  an instr to get written
      //executeDelay(instr) = number of EX stages occupied
      //N = number of EX stages in pipeline
      //writeDelay(instr) + executeDelay(instr) = N + 1
      //
      //for each instruction:
      //  for each read operand:
      //    end = how many cycles beforehand is its value available
      //    begin = how many cycles earlier is its writing instr issued
      //  if end > 0:
      //    ok
      //  else:
      //    if end == 0:
      //      forward from EX$begin
      //    if end == -1:
      //      forward
      //        
      //    forward from EX1
      //  if c == 1:
      //    forward from EX2
      //  if n > 0: 
      //     
      //    
      //is in reg file:
      //  begin >= N + 1
      //
      //available:
      //  begin >= executeDelay(instr)
      //
      //forward from what stage:
      //  EX$begin
      //
      //how many nops:
      //  executeDelay(instr) - begin
      
      //TODO 
      //1. annotate MI
      //  * annotation as extra preceding line (like the ones surrounding
      //    inline assembler blocks)
      //  * PreInstrSymbol
      //  * Encode in setFlags
      //2. defs/uses?
      //3. find executeDelays itineraryData[instr.getDesc().getSchedClass().getOperandLatency()].
      //4. unit test with assembler input??
      //5. implement annotations only
    }

    static char ID;
  };

  char ChipsScheduler::ID = 0;
}

namespace {
class ChipsPassConfig : public TargetPassConfig {
public:
  ChipsPassConfig(ChipsTargetMachine& TM, PassManagerBase& PM)
    : TargetPassConfig(TM, PM) { }

  ChipsTargetMachine& getChipsTargetMachine() const {
    return getTM<ChipsTargetMachine>();
  }

  void addIRPasses() override {
    TargetPassConfig::addIRPasses();
  }
  bool addInstSelector() override {
    addPass(createInstSelectorPass(getTM<ChipsTargetMachine>()));
    return false;
  }
  void addPreEmitPass() override {
    addPass(new ChipsScheduler());
  }
};
}

TargetPassConfig* ChipsTargetMachine::createPassConfig(
    PassManagerBase& PM) {
  return new ChipsPassConfig(*this, PM);
}


struct ChipsDAGToDAGISel : public SelectionDAGISel {
  ChipsSubtarget const* subtarget;

  explicit ChipsDAGToDAGISel(ChipsTargetMachine& tm) : SelectionDAGISel(tm) { }

  void Select(SDNode* n) override {
    // Necessary to make the selector ignore INSERT/EXTRACT_SUBREG machine nodes
    if(n->isMachineOpcode()) {
      n->setNodeId(-1);
      return;
    }
    SelectCode(n);
  }

  bool runOnMachineFunction(MachineFunction& func) override {
    subtarget = &func.getSubtarget<ChipsSubtarget>();
    return SelectionDAGISel::runOnMachineFunction(func);
  }

  bool SelectStackAddr(SDValue addr, SDValue& base) {
//    errs() << "[CHIPS] selecting addr\n";
//    errs() << "[TargetGlobalAddress] " << ISD::TargetGlobalAddress << " " <<
//      "[TargetGlobalTLSAddress] " << ISD::TargetGlobalTLSAddress << " " <<
//      "[Frameindex] " << ISD::Frame
//    if(addr.getOpcode() == ISD::TargetGlobalAddress)
//      errs() << "is global addr\n";

    if(auto frameIndexNode = dyn_cast<FrameIndexSDNode>(addr)) {
      base = CurDAG->getTargetFrameIndex(frameIndexNode->getIndex(),
          MVT::i32);
      //offset = CurDAG->getTargetConstant(0, SDLoc(addr), MVT::i32);
          //subtarget->getXLenVT());
      return true;
    }
    return false;
  }

#include "ChipsGenDAGISel.inc"
};

FunctionPass* createInstSelectorPass(ChipsTargetMachine& tm) {
  return new ChipsDAGToDAGISel(tm);
}

class ChipsMCRegOperand : public MCTargetExpr {
private:
  unsigned regNum;
  std::stringstream ss;

  ChipsMCRegOperand(unsigned regNum, MachineOperandInfo const* operandInfo) :
    regNum(regNum) {
    if(operandInfo->mustBeForwarded())
      ss << "foo" << operandInfo->getStageToForwardFrom();
  }

public:
  static ChipsMCRegOperand const* create(unsigned regNum,
      MachineOperandInfo const* operandInfo,
      MCContext& context) {
    return new (context) ChipsMCRegOperand(regNum, operandInfo);
  }

  unsigned getRegNum() const {
    return regNum;
  }

  void fixELFSymbolsInTLSFixups(MCAssembler& assembler) const override { }

  bool evaluateAsRelocatableImpl(MCValue& res, MCAsmLayout const* layout,
      MCFixup const* fixup) const override {
//    return subExpr->evaluateAsRelocatable(res, layout, fixup);
  }

  void visitUsedExpr(MCStreamer& streamer) const override {
//    streamer.visitUsedExpr(*subExpr);
  }

  MCFragment* findAssociatedFragment() const override {
//    return subExpr->findAssociatedFragment();
  }

  void printImpl(raw_ostream& out, MCAsmInfo const* asmInfo) const override {
//    out << ss.str();

//    subExpr->print(out, asmInfo);
  }
};

//TODO in ChipsMCInstLower.cpp
static MCOperand lowerSymbolOperand(MachineOperand const& operand,
    MCSymbol* sym, AsmPrinter const& printer) {
  MCContext& context = printer.OutContext;

  //TODO have a VariantKind
  //RISCVMCExpr::VariantKind Kind;

  MCSymbol const* symbol = nullptr;
  switch(operand.getType()) {
  default: llvm_unreachable("Unknown type in LowerSymbolOperand");
  case MachineOperand::MO_MachineBasicBlock:
    symbol = operand.getMBB()->getSymbol();
    break;
  case MachineOperand::MO_GlobalAddress:
    symbol = printer.getSymbol(operand.getGlobal());
    break;
  case MachineOperand::MO_BlockAddress:
    symbol = printer.GetBlockAddressSymbol(operand.getBlockAddress());
    break;
  case MachineOperand::MO_ExternalSymbol:
    symbol = printer.GetExternalSymbolSymbol(operand.getSymbolName());
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    symbol = printer.GetCPISymbol(operand.getIndex());
    break;
  }

//  switch (operand.getTargetFlags()) {
//  default:
//    llvm_unreachable("Unknown target flag on GV operand");
//  case RISCVII::MO_None:
//    Kind = RISCVMCExpr::VK_RISCV_None;
//    break;
//  case RISCVII::MO_LO:
//    Kind = RISCVMCExpr::VK_RISCV_LO;
//    break;
//  case RISCVII::MO_HI:
//    Kind = RISCVMCExpr::VK_RISCV_HI;
//    break;
//  }

  MCExpr const* expr = MCSymbolRefExpr::create(symbol, context);
  if(!operand.isJTI() && !operand.isMBB()) {
    int64_t offset = operand.getOffset();
    //already done in visitGetElementPtr
    //offset >>= 2;
    if(offset)
      expr = MCBinaryExpr::createAdd(expr, 
          MCConstantExpr::create(offset, context), context);
  }
//  const SparcMCExpr *expr = SparcMCExpr::create(Kind, MCSym,
//                                                AP.OutContext);
  return MCOperand::createExpr(expr);
}

bool lowerChipsMachineOperandToMCOperand(MachineOperand const& operand,
    MCOperand& mcOperand, MachineOperandInfo const* operandInfo,
    AsmPrinter const& printer) {
  switch(operand.getType()) {
  default:
    report_fatal_error("lowerChipsMachineInstrToMCInst: unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if(operand.isImplicit())
      return false;
    //mcOperand = MCOperand::createReg(operand.getReg());
    mcOperand = MCOperand::createExpr(ChipsMCRegOperand::create(
          operand.getReg(),
          operandInfo,
          printer.OutContext));
    break;
  case MachineOperand::MO_RegisterMask:
    // Regmasks are like implicit defs.
    return false;
  case MachineOperand::MO_Immediate:
    mcOperand = MCOperand::createImm(operand.getImm());
    break;
  case MachineOperand::MO_MachineBasicBlock:
    mcOperand = lowerSymbolOperand(operand,
        operand.getMBB()->getSymbol(), printer);
    break;
  case MachineOperand::MO_GlobalAddress:
    mcOperand = lowerSymbolOperand(operand,
        printer.getSymbol(operand.getGlobal()), printer);
    break;
  case MachineOperand::MO_BlockAddress:
    mcOperand = lowerSymbolOperand(operand,
        printer.GetBlockAddressSymbol(operand.getBlockAddress()), printer);
    break;
  case MachineOperand::MO_ExternalSymbol:
    mcOperand = lowerSymbolOperand(operand,
        printer.GetExternalSymbolSymbol(operand.getSymbolName()), printer);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    mcOperand = lowerSymbolOperand(operand,
        printer.GetCPISymbol(operand.getIndex()), printer);
    break;
  }
  return true;
}

void lowerChipsMachineInstrToMCInst(MachineInstr const* instr, MCInst& mcInstr,
    const AsmPrinter& printer) {
  mcInstr.setOpcode(instr->getOpcode());

  MachineFuncInfo const* funcInfo = instr->getParent()->getParent()->getInfo<
    MachineFuncInfo>();
  std::vector<MachineOperandInfo> const& operandInfos = funcInfo->
    getOperandInfos(instr);
  assert(operandInfos.size() == instr->getNumOperands());
  auto operandInfo = operandInfos.begin();
  for(MachineOperand const& operand : instr->operands()) {
    assert(operandInfo != operandInfos.end());
    MCOperand mcOperand;
    if(lowerChipsMachineOperandToMCOperand(operand, mcOperand, &*operandInfo,
          printer))
      mcInstr.addOperand(mcOperand);
    ++operandInfo;
  }
}

struct ChipsAsmPrinter : public AsmPrinter {
  ChipsAsmPrinter(TargetMachine& tm, std::unique_ptr<MCStreamer> streamer)
    : AsmPrinter(tm, std::move(streamer)) { }

  void EmitInstruction(MachineInstr const* instr) override {
    //instr->dump();
    MCInst mcInstr;
//    emitForwardingInfo(instr);
    lowerChipsMachineInstrToMCInst(instr, mcInstr, *this);
    EmitToStreamer(*OutStreamer, mcInstr);
  }

//  void emitForwardingInfo(MachineInstr const* instr) {
//    OutStreamer->EmitRawText(Twine("\t\t#foo"));
//  }

  bool PrintAsmOperand(MachineInstr const* instr,
      unsigned opNum, unsigned asmVariant, char const* extraCode,
      raw_ostream& out) override;
};

struct ChipsMCAsmInfo : public MCAsmInfoELF {
  //ChipsMCAsmInfo(Triple& triple) {
  ChipsMCAsmInfo() {
    HasDotTypeDotSizeDirective = false;
    //ZeroDirective = nullptr;
    Data8bitsDirective = "\t.byte\t";
    Data16bitsDirective = nullptr;
    Data32bitsDirective = "\t.word\t";
    Data64bitsDirective = nullptr;
    AsciiDirective = nullptr;
    AscizDirective = nullptr;
//    CodePointerSize = 1;
//    CalleeSaveStackSlotSize = 1;
    IsLittleEndian = false;
//    SupportsDebugInformation = true;
  }
};

static MCAsmInfo* createChipsMCAsmInfo(MCRegisterInfo const& registers,
    Triple const& triple) {
  return new ChipsMCAsmInfo();
}

struct ChipsInstPrinter : public MCInstPrinter {
  ChipsInstPrinter(MCAsmInfo const& mcAsmInfo, MCInstrInfo const& mcInstrInfo,
      MCRegisterInfo const& mcRegisterInfo) :
    MCInstPrinter(mcAsmInfo, mcInstrInfo, mcRegisterInfo) { }

  void printRegName(raw_ostream& out, unsigned regNum) const override {
    out << "%" << StringRef(getRegisterName(regNum)).lower();
  }
  void printInst(MCInst const* instr, raw_ostream& out, StringRef annotation,
      MCSubtargetInfo const& subtargetInfo) override {
    if (!printAliasInstr(instr, subtargetInfo, out))
      printInstruction(instr, subtargetInfo, out);
  }

  // Autogenerated by tblgen.
  void printInstruction(const MCInst *MI, const MCSubtargetInfo &STI,
                        raw_ostream &O);
  static const char *getRegisterName(unsigned RegNo);
  bool printAliasInstr(MCInst const* instr, MCSubtargetInfo const& subtarget,
      raw_ostream& out);

//  void printCustomAliasOperand(const MCInst *MI, unsigned OpIdx,
//                               unsigned PrintMethodIdx,
//                               const MCSubtargetInfo &STI, raw_ostream &O);

  void printOperand(MCInst const* instr, int num,
      MCSubtargetInfo const& subtarget, raw_ostream& out) {
    MCOperand const& operand = instr->getOperand(num);

    if(operand.isReg()) {
      printRegName(out, operand.getReg());
      return;
    }

    if(operand.isImm()) {
      out << (int)operand.getImm();
      return;
    }

    if(operand.isExpr() && MCTargetExpr::classof(operand.getExpr())) {
      ChipsMCRegOperand const* reg = static_cast<ChipsMCRegOperand const*>(
          operand.getExpr());
      printRegName(out, reg->getRegNum());
      reg->print(out, &MAI);
      return;
    }

    assert(operand.isExpr() && "Unknown operand kind in printOperand");
    operand.getExpr()->print(out, &MAI);
  }
//  void printMemOperand(const MCInst *MI, int opNum, const MCSubtargetInfo &STI,
//                       raw_ostream &OS, const char *Modifier = nullptr);
//  void printCCOperand(const MCInst *MI, int opNum, const MCSubtargetInfo &STI,
//                      raw_ostream &OS);
//  bool printGetPCX(const MCInst *MI, unsigned OpNo, const MCSubtargetInfo &STI,
//                   raw_ostream &OS);
};

#define GET_INSTRUCTION_NAME
#define PRINT_ALIAS_INSTR
#include "ChipsGenAsmWriter.inc"

static MCInstPrinter* createChipsMCInstPrinter(
    Triple const & T,
    unsigned SyntaxVariant,
    MCAsmInfo const& MAI,
    MCInstrInfo const& MII,
    MCRegisterInfo const& MRI) {
  return new ChipsInstPrinter(MAI, MII, MRI);
}

bool ChipsAsmPrinter::PrintAsmOperand(MachineInstr const* instr,
    unsigned opNum, unsigned asmVariant, char const* extraCode,
    raw_ostream& out) {
  if(extraCode && extraCode[0]) {
    if(extraCode[1] != 0)
      return true; // Unknown modifier.

    if(extraCode[0] != 'f' && extraCode[0] != 'r')
      return AsmPrinter::PrintAsmOperand(instr, opNum, asmVariant, extraCode,
          out);
  }

  //printOperand(instr, opNo, out);
  MachineOperand const& operand = instr->getOperand(opNum);
  out << "%" << StringRef(ChipsInstPrinter::getRegisterName(
        operand.getReg())).lower();
  return false;
}

RegisterAsmPrinter<ChipsAsmPrinter> Z(chips_target);
RegisterMCAsmInfoFn W(chips_target, createChipsMCAsmInfo);

class RegisterAll {
public:
  RegisterAll() {
    TargetRegistry::RegisterMCInstPrinter(chips_target,
        createChipsMCInstPrinter);

    stackSizesOut.open("out.backend.stack-sizes",
        std::ofstream::binary | std::ofstream::trunc);
  }
};

RegisterAll X2;

//bool match_arch(Triple::ArchType arch_type) {
//  return true;
//}

//static Triple constexpr triple = Triple(
//    Triple::ArchType::UnknownArch,
//    Triple::EnvironmentType::EABI,
//    Triple::ObjectFormatType::UnknownObjectFormat,
//    Triple::OSType::UnknownOS);

//extern "C" void LLVMInitializeChipsTargetInfo() {
//  RegisterTarget<Triple::UnknownArch, /*HasJIT=*/false>
//        X(chips_target, "chips", "Chips", "chips");
//  std::cerr << "lala\n";
//}
//extern "C" void LLVMInitializeChipsTarget() {
//  RegisterTargetMachine<ChipsTargetMachine> X(chips_target);
//  std::cerr << "lala\n";
//}
RegisterTarget<Triple::UnknownArch, /*HasJIT=*/false>
      X(chips_target, "chips", "Chips", "chips");
RegisterTargetMachine<ChipsTargetMachine> Y(chips_target);
