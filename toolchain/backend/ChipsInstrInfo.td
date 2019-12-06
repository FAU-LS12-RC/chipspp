include "ChipsInstrFormats.td"

def Stop : ChipsInstr<
  (outs),
  (ins),
  "stop",
  []>;

def Nop : ChipsInstr<
  (outs),
  (ins),
  "nop",
  []>;

def brtarget : Operand<OtherVT> {
  //let EncoderMethod = "getBranchTargetOpValue";
}

let isTerminator = 1, isBranch = 1 in
def Goto : ChipsInstr<
  (outs),
  (ins brtarget:$target),
  "goto $target",
  [(br bb:$target)]>;

//TODO AluInstr
def ShiftLeft : ChipsInstr<
  (outs IntRegs:$out),
  (ins IntRegs:$fst, IntRegs:$snd),
  "shift_left $out, $fst, $snd",
  [(set IntRegs:$out, (shl IntRegs:$fst, IntRegs:$snd))]>;

//greater $r, A, B
//jmp_if_true _, $r, $target

//def : Pat<
//  (br_cc $lhs, $op, $rhs, $true, $false),
//  (Stop)>;
  
//def : Pat<(i16 (sext (i8 (X86setcc_c X86_COND_B, EFLAGS)))),
//          (SETB_C16r)>;
//def : Pat<
//  (setcc $lhs, $rhs, $condCode),
//  (

//class ConditionInstr<string mnemonic> :
//    ChipsInstr<
//      (outs Regs:$out),
//      (ins Regs:$lhs, Regs:$rhs),
//      !strconcat(mnemonic, " $out, $lhs, $rhs"),
//      []>;
//multiclass ConditionalJump<string mnemonic, CondCode condCode> {
//  def i : ConditionInstr<mnemonic>;
//  def : Pat<
//    (brcond (i32 (setcc Regs:$lhs, Regs:$rhs, condCode)), bb:$target),
//    (JmpIfTrue (i Regs:$lhs, Regs:$rhs), $target)>;
//}

//TODO refactor

class ConditionInstr<string mnemonic, CondCode condCode, RegisterClass regs> :
    ChipsInstr<
      (outs IntRegs:$out),
      (ins regs:$lhs, regs:$rhs),
      !strconcat(mnemonic, " $out, $lhs, $rhs"),
      [(set IntRegs:$out, (setcc regs:$lhs, regs:$rhs, condCode))]>;
multiclass ConditionInstrs<string mnemonic, CondCode condCode> {
  def Int : ConditionInstr<mnemonic, condCode, IntRegs>;
  def Fp : ConditionInstr<mnemonic, condCode, FpRegs>;
}
multiclass ReversibleConditionInstrs<string mnemonic, CondCode condCode,
    CondCode reversedCondCode> {
  def Int : ConditionInstr<mnemonic, condCode, IntRegs>;
  def Fp : ConditionInstr<mnemonic, condCode, FpRegs>;
  def : Pat<
    (setcc IntRegs:$lhs, IntRegs:$rhs, reversedCondCode),
    (!cast<ChipsInstr>(NAME#Int) $rhs, $lhs)>;
  def : Pat<
    (setcc FpRegs:$lhs, FpRegs:$rhs, reversedCondCode),
    (!cast<ChipsInstr>(NAME#Fp) $rhs, $lhs)>;
}

defm UnsignedGreater : ReversibleConditionInstrs<
  "unsigned_greater", SETUGT, SETULT>;
defm Greater : ReversibleConditionInstrs<"greater", SETGT, SETLT>;
defm GreaterEqual : ReversibleConditionInstrs<"greater_equal", SETGE, SETLE>;
defm UnsignedGreaterEqual : ReversibleConditionInstrs<
  "unsigned_greater_equal", SETUGE, SETULE>;
defm Equal : ConditionInstrs<"equal", SETEQ>;

//TODO test for NaN
//def FpEqual : FpConditionInstr<"equal", SETOEQ>;
defm NotEqual : ConditionInstrs<"not_equal", SETNE>;

//TODO this is WRONG in general
def : Pat<
  (setcc FpRegs:$lhs, FpRegs:$rhs, SETOEQ),
  (EqualFp $lhs, $rhs)>;
def : Pat<
  (setcc FpRegs:$lhs, FpRegs:$rhs, SETOLT),
  (GreaterFp $rhs, $lhs)>;
def : Pat<
  (setcc FpRegs:$lhs, FpRegs:$rhs, SETOGT),
  (GreaterFp $lhs, $rhs)>;
def : Pat<
  (setcc FpRegs:$lhs, FpRegs:$rhs, SETUNE),
  (NotEqualFp $lhs, $rhs)>;
def : Pat<
  (setcc FpRegs:$lhs, FpRegs:$rhs, SETOGE),
  (GreaterEqualFp $lhs, $rhs)>;
def : Pat<
  (setcc FpRegs:$lhs, FpRegs:$rhs, SETOLE),
  (GreaterEqualFp $rhs, $lhs)>;

//def FpUnsignedGreaterEqual : FpConditionInstr<"

def StackAddr : ComplexPattern<iPTR, 1, "SelectStackAddr", [frameindex], []>;

def Mem : Operand<iPTR> {
//  let PrintMethod = "printMemOperand";
//  let MIOperandInfo = (ops ptr_rc, ptr_rc);
  let MIOperandInfo = (ops ptr_rc);
//  let ParserMatchClass = SparcMEMrrAsmOperand;
}

//TODO loading i32 requires literal_hi
let isNotDuplicable = 0, isMoveImm = 1, isReMaterializable = 1 in {
  def Movl : ChipsInstr<
    (outs IntRegs:$dst),
    (ins Operand<i32>:$imm),
    "movl $dst, $imm",
    [(set IntRegs:$dst, imm:$imm)]>;
}

multiclass DFpUnidirectionalMov<string name, ChipsReg register> {
  let Defs = [register] in
    def In : ChipsInstr<(outs), (ins AllRegs:$in),
        !strconcat(name#_in, " $in"), []>;
  let Uses = [register] in
    def Out : ChipsInstr<(outs AllRegs:$out), (ins),
        !strconcat(name#_out, " $out"), []>;
}
multiclass DFpMov<string name, ChipsReg register> {
  defm Hi : DFpUnidirectionalMov<name#_hi, register>;
  defm Lo : DFpUnidirectionalMov<name#_lo, register>;
}
defm MovA : DFpMov<"a", RA>;
defm MovB : DFpMov<"b", RB>;

//TODO put instructions into categories (see doc)?

//TODO refactor: put load/store patterns into pat defs
//               use i32imm operands
//TODO use SDNodeXForm
//TODO use hasSideEffects
//TODO define aliases

//TODO refactor
def FloatAdd : ChipsInstr<
  (outs FpRegs:$dst),
  (ins FpRegs:$lhs, FpRegs:$rhs),
  "float_add $dst, $lhs, $rhs",
  [(set FpRegs:$dst, (fadd FpRegs:$lhs, FpRegs:$rhs))]>;
def FloatSubtract : ChipsInstr<
  (outs FpRegs:$dst),
  (ins FpRegs:$lhs, FpRegs:$rhs),
  "float_subtract $dst, $lhs, $rhs",
  [(set FpRegs:$dst, (fsub FpRegs:$lhs, FpRegs:$rhs))]>;
def FloatMultiply : ChipsInstr<
  (outs FpRegs:$dst),
  (ins FpRegs:$lhs, FpRegs:$rhs),
  "float_multiply $dst, $lhs, $rhs",
  [(set FpRegs:$dst, (fmul FpRegs:$lhs, FpRegs:$rhs))]>;
def FloatDivide : ChipsInstr<
  (outs FpRegs:$dst),
  (ins FpRegs:$lhs, FpRegs:$rhs),
  "float_divide $dst, $lhs, $rhs",
  [(set FpRegs:$dst, (fdiv FpRegs:$lhs, FpRegs:$rhs))]>;

def DoubleAdd : ChipsInstr<
  (outs DFpResultRegs:$dst),
  (ins DFpRegs:$lhs, DFpRegs:$rhs),
  "long_float_add",
  [(set DFpResultRegs:$dst, (fadd DFpRegs:$lhs, DFpRegs:$rhs))]>;
def DoubleSub : ChipsInstr<
  (outs DFpResultRegs:$dst),
  (ins DFpRegs:$lhs, DFpRegs:$rhs),
  "long_float_subtract",
  [(set DFpResultRegs:$dst, (fsub DFpRegs:$lhs, DFpRegs:$rhs))]>;

//TODO ChipsCallSeqStart
def SDT_SPCallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>,
                                          SDTCisVT<1, i32> ]>;
def SDT_SPCallSeqEnd   : SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                        SDTCisVT<1, i32> ]>;
def SDT_AddrWrapper : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;
def AddrWrapper : SDNode<"ChipsISD::ADDR_WRAPPER", SDT_AddrWrapper>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_SPCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_SPCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
   : ChipsInstr<outs, ins, asmstr, pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}

let Defs = [R3], Uses = [R3] in {
  def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                "!ADJCALLSTACKDOWN $amt1, $amt2",
                                [(callseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              "!ADJCALLSTACKUP $amt1",
                              [(callseq_end timm:$amt1, timm:$amt2)]>;
}

def SDT_LONG_TO_DOUBLE : SDTypeProfile<1, 2, []>;
def ChipsLongToDouble : SDNode<"ChipsISD::LONG_TO_DOUBLE", SDT_LONG_TO_DOUBLE>;
def SDT_DOUBLE_TO_LONG : SDTypeProfile<2, 1, []>;
def ChipsDoubleToLong : SDNode<"ChipsISD::DOUBLE_TO_LONG", SDT_DOUBLE_TO_LONG>;

def SDT_ChipsCall : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def call : SDNode<"ChipsISD::CALL", SDT_ChipsCall,
  [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;
let Defs = [R6], isCall = 1 in
def CALL : ChipsInstr<
  //(outs Regs:$addressReg),
  (outs),
  (ins i32imm:$target), //TODO not i32
  //"call $addressReg, $target",
  "call %r6, $target",
  []>;
def : Pat<
  (call tglobaladdr:$dst),
  (CALL $dst)>;
//only sensible if functions like memset are always linked to
//def : Pat<
//  (call texternalsym:$dst),
//  (CALL $dst)>;

class AluInstr<string mnemonic, SDNode operator> :
    ChipsInstr<
      (outs IntRegs:$out),
      (ins IntRegs:$lhs, IntRegs:$rhs),
      !strconcat(mnemonic, " $out, $lhs, $rhs"),
      [(set IntRegs:$out, (operator IntRegs:$lhs, IntRegs:$rhs))],
      IIC_ALU>,
    Sched<[WriteALU, ReadALU, ReadALU]>;

def Or : AluInstr<"or", or>;
def And : AluInstr<"and", and>;
def Xor : AluInstr<"xor", xor>;
let Uses = [Carry], Defs = [Carry] in {
  def AddWithCarry : AluInstr<"add_with_carry", adde>;
}
def Modulo : AluInstr<"modulo", srem>;
def UnsignedModulo : AluInstr<"unsigned_modulo", urem>;
let Defs = [Carry] in {
  def ShiftRight : AluInstr<"shift_right", sra>;
  def UnsignedShiftRight : AluInstr<"unsigned_shift_right", srl>;
  def Add : AluInstr<"add", add>;
  def Subtract : AluInstr<"subtract", sub>;
  //let Itinerary = IIC_two_cycle in 
    def Multiply : AluInstr<"multiply", mul>;
  //def MulHiSigned : AluInstr<"multiply", mulhs>;
//  def SignedMulLoHi : ChipsInstr<
//    (outs IntRegs:$lo, IntRegs:$hi),
//    (ins IntRegs:$lhs, IntRegs:$rhs),
//    "multiply $lo, $lhs, $rhs\n\tcarry $hi",
//    [(set i32:$lo, i32:$hi, (smullohi IntRegs:$lhs, IntRegs:$rhs))]>;
//  def UnsignedMulLoHi : ChipsInstr<
//    (outs IntRegs:$lo, IntRegs:$hi),
//    (ins IntRegs:$lhs, IntRegs:$rhs),
//    "multiply $lo, $lhs, $rhs\n\tcarry $hi",
//    [(set i32:$hi, i32:$lo, (umullohi IntRegs:$lhs, IntRegs:$rhs))]>;
  def MulHiUnsigned : ChipsInstr<
    (outs IntRegs:$hi),
    (ins IntRegs:$lhs, IntRegs:$rhs),
    "multiply %r12, $lhs, $rhs\n\tcarry $hi",
    [(set i32:$hi, (mulhu IntRegs:$lhs, IntRegs:$rhs))]>;
}
def : Pat<(addc IntRegs:$lhs, IntRegs:$rhs),
    (Add $lhs, $rhs)>;

let Defs = [RA] in {
  def IntToFloat : ChipsInstr<
    (outs AllRegs:$dst),
    (ins IntRegs:$src),
    "a_lo_in $src\n\tint_to_float\n\ta_lo_out $dst",
    [(set f32:$dst, (sint_to_fp IntRegs:$src))]>;
  def FloatToInt : ChipsInstr<
    (outs IntRegs:$dst),
    (ins AllRegs:$src),
    "a_lo_in $src\n\tfloat_to_int\n\ta_lo_out $dst",
    [(set i32:$dst, (fp_to_sint AllRegs:$src))]>;
  def DoubleToFloat : ChipsInstr<
    (outs FpRegs:$dst),
    (ins DFpResultRegs:$src),
    "double_to_float\n\ta_lo_out $dst",
    [(set f32:$dst, (fpround f64:$src))]>;
    //[(set f64:$dst, (sint_to_fp (i64 (build_pair i32:$srcHi, i32:$srcLo))))]>;
}
def FloatToDouble : ChipsInstr<
  (outs DFpResultRegs:$dst),
  (ins FpRegs:$src),
  "a_lo_in $src\n\tfloat_to_double",
  [(set f64:$dst, (fpextend f32:$src))]>;
def LongToDouble : ChipsInstr<
  (outs DFpResultRegs:$dst),
  (ins IntRegs:$srcHi, IntRegs:$srcLo),
  "a_lo_in $srcLo\n\ta_hi_in $srcHi\n\tlong_to_double",
  [(set f64:$dst, (ChipsLongToDouble IntRegs:$srcHi, IntRegs:$srcLo))]>;
  //TODO fp_to_uint: Negative values must become zero
def DoubleToLong : ChipsInstr<
  (outs IntRegs:$dstHi, IntRegs:$dstLo),
  (ins DFpResultRegs:$src),
  "double_to_long\n\ta_hi_out $dstHi\n\ta_lo_out $dstLo",
  [(set i32:$dstHi, i32:$dstLo, (ChipsDoubleToLong DFpResultRegs:$src))]>;


def : Pat<
  (f32 (bitconvert i32:$value)),
  (COPY_TO_REGCLASS $value, FpRegs)>;
def : Pat<
  (i32 (bitconvert f32:$value)),
  (COPY_TO_REGCLASS $value, IntRegs)>;

//def : Pat<(i64 (anyext i32:$val)), (COPY_TO_REGCLASS $val, I64Regs)>;
//def : Pat<(i32 (trunc i64:$val)), (COPY_TO_REGCLASS $val, IntRegs)>;
//def : Pat<(i32 (anyext i16:$val)), (COPY_TO_REGCLASS $val, IntRegs)>;
//def : Pat<(i16 (trunc i32:$val)), (COPY_TO_REGCLASS $val, IntRegs)>;
//def : Pat<(i16 (trunc i32:$val)), (COPY_TO_REGCLASS $val, IntRegs)>;
//def : Pat<
//  (setcc i16:$lhs, (trunc i32:$rhs), SETULT),
//  (

//def : Pat<
//  (f32 (bitconvert i32:$value)),
//  (f32 $value)>;

// "LEA" forms of add (patterns to make tblgen happy)
let isCodeGenOnly = 1 in {
  def LeaPseudo : ChipsInstr<
    (outs IntRegs:$dst),
    (ins Mem:$addr),
    "addl $dst, $addr, 0",
    [(set iPTR:$dst, StackAddr:$addr)]>;
//  def LeaPseudo2 : ChipsInstr<
//    (outs IntRegs:$dst),
//    (ins Mem:$addr),
//    "addl $dst, $addr, 0",
//    [(set IntRegs:$dst, tglobaladdr:$addr)]>;
}

def Select : ChipsInstr<
  (outs IntRegs:$dst),
  (ins IntRegs:$bool, IntRegs:$true, IntRegs:$false),
  "select $dst, $bool, $true, $false",
  [(set IntRegs:$dst, (select IntRegs:$bool, IntRegs:$true, IntRegs:$false))]>;
def FpSelect : ChipsInstr<
  (outs FpRegs:$dst),
  (ins IntRegs:$bool, FpRegs:$true, FpRegs:$false),
  "select $dst, $bool, $true, $false",
  [(set FpRegs:$dst, (select IntRegs:$bool, FpRegs:$true, FpRegs:$false))]>;

//class Branch<string asmString> : ChipsInstr<
//  (outs),
//  (ins IntRegs:$lhs, IntRegs:$rhs, IntRegs:$target),
//  asmString,
//  []> {
//  let isBranch = 1;
//}

def retflag : SDNode<"ChipsISD::RET_FLAG", SDTNone,
    [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
let isReturn = 1, isTerminator = 1, isBarrier = 1, Uses = [R6] in 
def Return : ChipsInstr<
  (outs),
  (ins IntRegs:$target),
  "return $target",
  []>;
def : Pat<(retflag), (Return R6)>;

def simm16 : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
def simm16Op : Operand<i32>;
def simm14 : PatLeaf<(imm), [{ return isInt<14>(N->getSExtValue()); }]>;
def simm14Op : Operand<i32>;
def simm13 : PatLeaf<(imm), [{ return isInt<13>(N->getSExtValue()); }]>;
def simm13Op : Operand<i32>;
//def LO10 : SDNodeXForm<imm, [{
//  return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 1023, SDLoc(N),
//                                   MVT::i32);
//}]>;

//args: NumResults, NumOperands, Constraints
//def SDTChipsAddl : SDTypeProfile<1, 2, []>;
//def addl : SDNode<"ChipsISD::ADDL", SDTChipsAddl, []>;
//TODO make commutative

let isReMaterializable = 1 in {
  def Addl : ChipsInstr<
    (outs IntRegs:$dst),
    (ins IntRegs:$reg, simm16Op:$imm),
    "addl $dst, $reg, $imm",
    [(set IntRegs:$dst, (add IntRegs:$reg, simm16:$imm))]>;
}

//def : Pat<(StackAddr:$addr), (Addl $addr, 0)>;

def JmpAddr : Operand<OtherVT>;

//def : Pat<(add tglobaladdr:$base, i32:$offset), (Addl (Movl $base), $offset)>;

let isBranch = 1, isTerminator = 1 in
def JmpIfTrue : ChipsInstr<
  (outs),
  (ins IntRegs:$boolean, JmpAddr:$target),
  "jmp_if_true $boolean, $target",
  [(brcond IntRegs:$boolean, bb:$target)]>;

class LoadInstr<string mnemonic, Operand displacementOp> :
    ChipsInstr<
        (outs AllRegs:$dst),
        (ins Mem:$addr, displacementOp:$disp),
        !strconcat(mnemonic, " $dst, [$addr], $disp"),
        [], IIC_load>,
    Sched<[WriteLoad, ReadALU]>;

let mayLoad = 1, isAsCheapAsAMove = 0 in {
  def LoadDisp : LoadInstr<"load_disp", simm13Op>;
  def Load8Disp : LoadInstr<"load8", simm13Op>;
  def Load8Signed : LoadInstr<"load8_signed", simm13Op>;
  def Load16Disp : LoadInstr<"load16", simm13Op>;
  def Load16Signed : LoadInstr<"load16_signed", simm13Op>;
}
let mayStore = 1 in {
  def Store : ChipsInstr<
    (outs),
    (ins Mem:$addr, AllRegs:$src, simm14Op:$disp),
    "store [$addr], $src, $disp",
    []>;
  def Store8 : ChipsInstr<
    (outs),
    (ins Mem:$addr, AllRegs:$src, simm14Op:$disp),
    "store8 [$addr], $src, $disp",
    []>;
  def Store16 : ChipsInstr<
    (outs),
    (ins Mem:$addr, AllRegs:$src, simm14Op:$disp),
    "store16 [$addr], $src, $disp",
    []>;

//  def StoreDouble : ChipsInstr<
//    (outs),
//    (ins IntRegs:$addr, DFpRegs:$src, simm14Op:$disp),
//    "store_double [$addr], $src, $disp",
//    []>;
}

//let mayLoad = 1 in
//def Load : ChipsInstr<
//  (outs AllRegs:$dst),
//  (ins IntRegs:$addr),
//  "load $dst, [$addr]",
//  []>;
//let mayStore = 1 in
//def Store : ChipsInstr<
//  (outs),
//  (ins IntRegs:$addr, AllRegs:$src),
//  "store [$addr], $src",
//  []>;

//let mayStore = 1 in
//def TruncStore : ChipsInstr<
//  (outs),
//  (ins IntRegs:$addr, DFpRegs:$src),
//  "truncstore [$addr], $src",
//  []>;

//TODO refactor
def : Pat<
  (store (AddrWrapper tglobaladdr:$src), IntRegs:$addr),
  (Store $addr, (Movl $src), 0)>;

multiclass Loads<ValueType vt, PatFrag frag, ChipsInstr loadOp> {
  def : Pat<(vt (frag (AddrWrapper tconstpool:$addr))),
      (loadOp (Movl 0), $addr)>;
  def : Pat<(vt (frag (AddrWrapper tglobaladdr:$addr))),
      (loadOp (Movl 0), $addr)>;
  def : Pat<(vt (frag StackAddr:$addr)), (loadOp $addr, 0)>;
  def : Pat<(vt (frag IntRegs:$addr)), (loadOp $addr, 0)>;
}
multiclass Stores<ValueType vt, PatFrag frag, ChipsInstr storeOp> {
  //def : Pat<(frag vt:$src, tconstpool:$addr), (storeOp (Movl $addr), $src, 0)>;
  def : Pat<(frag vt:$src, (AddrWrapper tconstpool:$addr)),
      (storeOp (Movl 0), $src, $addr)>;
  def : Pat<
          (frag vt:$src, (AddrWrapper tglobaladdr:$addr)),
          (storeOp (Movl 0), $src, $addr)>;
  def : Pat<(frag vt:$src, StackAddr:$addr), (storeOp $addr, $src, 0)>;
  def : Pat<(frag vt:$src, IntRegs:$addr), (storeOp $addr, $src, 0)>;
}
defm : Loads<i32, load, LoadDisp>;
defm : Stores<i32, store, Store>;
defm : Loads<f32, load, LoadDisp>;
defm : Stores<f32, store, Store>;

defm : Loads<i32, zextloadi8, Load8Disp>;
defm : Loads<i32, sextloadi8, Load8Signed>;
defm : Loads<i32, extloadi8, Load8Disp>;
defm : Loads<i32, zextloadi16, Load16Disp>;
defm : Loads<i32, extloadi16, Load16Disp>;
defm : Loads<i32, sextloadi16, Load16Signed>;
defm : Stores<i32, truncstorei8, Store8>;
defm : Stores<i32, truncstorei16, Store16>;

//defm : Stores<f64, store, StoreDouble>;
//defm : Loads<f64, load, LoadDouble>;

def : Pat<(add (AddrWrapper tconstpool:$addr), IntRegs:$rhs),
    (Add (Movl $addr), $rhs)>;
def : Pat<(add (AddrWrapper tglobaladdr:$addr), IntRegs:$rhs),
    (Addl $rhs, $addr)>;
def : Pat<(AddrWrapper tglobaladdr:$addr), (Movl $addr)>;

//Cannot select: 0x555f99c68968: i32,ch = load<LD2[%17+2], anyext from i16> 0x555f99c28b10, 0x555f99c683b8, undef:i32
//def : Pat<(i32 (extloadi16 IntRegs:$addr)), (LoadDisp $addr, 0)>;

//def : Pat<(truncstorei32 i64:$src, IntRegs:$addr), (TruncStore $addr, $src)>;


//def : Pat<(tglobaladdr:$in), (i32 $in)>;

//def : Pat<(setgt Regs:$lhs, Regs:$rhs), (Greater Regs:$lhs, Regs:$rhs)>;


//    def _1 : Ii8PCRel <opc1, RawFrm, (outs), (ins brtarget8:$dst), asm,
//                       [(X86brcond bb:$dst, Cond, EFLAGS)]>;

//def X86cmp     : SDNode<"X86ISD::CMP" ,     SDTX86CmpTest>;

