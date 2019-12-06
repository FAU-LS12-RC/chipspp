def IIC_ALU : InstrItinClass;
def IIC_load : InstrItinClass;

def PIPELINE : FuncUnit; // normal five-stage pipeline

def ChipsItineraries : ProcessorItineraries<[PIPELINE], [], [
  InstrItinData<IIC_ALU, [InstrStage<1, [PIPELINE]>], [1, 1, 1]>,
  InstrItinData<IIC_load, [InstrStage<2, [PIPELINE]>], [2, 1]>
]>;

def ChipsGenericModel : SchedMachineModel {
  int IssueWidth = 1;
  int MicroOpBufferSize = 0;

  // These figures assume an L1 hit.
  int LoadLatency = 2;
//  int MispredictPenalty = 4;
//
//  int HighLatency = 37;
  list<Predicate> UnsupportedFeatures = [];

  let CompleteModel = 0;
  let PostRAScheduler = 1;

  let Itineraries = ChipsItineraries;
}

// From TargetSchedule.td:
// incomplete. By default, resources are consumed for a single cycle,
// regardless of latency, which models a fully pipelined processing
// unit. A value of 0 for ResourceCycles means that the resource must
// be available but is not consumed, which is only relevant for
// unbuffered resources.

//Note: Use BufferSize = 1 ?
//Instructions should be allowed to be dispatched any time but their
//results are only available after a certain latency.

//===----------------------------------------------------------------------===//
// Define each kind of processor resource and number available.
//
// The AGU unit has BufferSize=1 so that the latency between operations
// that use it are considered to stall other operations.
//
// The FP unit has BufferSize=0 so that it is a hard dispatch
// hazard. No instruction may be dispatched while the unit is reserved.

def WriteALU : SchedWrite;
def ReadALU : SchedRead;

def WriteLoad : SchedWrite;

let SchedModel = ChipsGenericModel in {

//def FpAdder : ProcResource<1>;
//def FpMultiplier : ProcResource<1>;
def ALU : ProcResource<1> { let BufferSize = 0; }
//TODO is this another resource? -> Super resource: pipeline?
def LoadStore : ProcResource<1> { let BufferSize = 0; }
//def A9UnitMul : ProcResource<1> { let Super = A9UnitALU; }
//def A9UnitAGU : ProcResource<1> { let BufferSize = 1; }
//def A9UnitLS  : ProcResource<1>;
//def A9UnitFP  : ProcResource<1> { let BufferSize = 0; }
//def A9UnitB   : ProcResource<1>;

// Consume an issue slot, but no processor resources. This is useful when all
// other writes associated with the operand have NumMicroOps = 0.
//def A9WriteIssue : SchedWriteRes<[]> { let Latency = 0; }

def ChipsWriteALU : SchedWriteRes<[ALU]> { let Latency = 1; }
def ChipsWriteLoad : SchedWriteRes<[LoadStore, ALU]> { let Latency = 2; }

def : ReadAdvance<ReadALU, 0>;
//def ChipsReadALU : SchedReadAdvance<0>;
//def : SchedAlias<ReadALU, ChipsReadALU>;

def : SchedAlias<WriteALU, ChipsWriteALU>;
def : SchedAlias<WriteLoad, ChipsWriteLoad>;

//TODO define loads
//-> write test program that does many loads
//-> see if nops are inserted

//TODO define ResourceCycles = [x] for FPU to prevent dispatch
//to a unpipelined and busy FPU

//def : SchedAlias<WriteMUL16, A9WriteM16>;
//def : SchedAlias<WriteMUL32, A9WriteM>;
//def : SchedAlias<WriteMUL64Lo, A9WriteM>;
//def : SchedAlias<WriteMUL64Hi, A9WriteMHi>;
//def : SchedAlias<WriteMAC16, A9WriteM16>;
//def : SchedAlias<WriteMAC32, A9WriteM>;
//def : SchedAlias<WriteMAC64Lo, A9WriteM>;
//def : SchedAlias<WriteMAC64Hi, A9WriteMHi>;
//def : ReadAdvance<ReadMUL, 0>;
//def : ReadAdvance<ReadMAC, 0>;

// Floating-point
// Only one FP or AGU instruction may issue per cycle. We model this
// by having FP instructions consume the AGU resource.
//def A9WriteF      : SchedWriteRes<[A9UnitFP, A9UnitAGU]> { let Latency = 4; }
//def A9WriteFMov   : SchedWriteRes<[A9UnitFP, A9UnitAGU]> { let Latency = 1; }
//def A9WriteFMulS  : SchedWriteRes<[A9UnitFP, A9UnitAGU]> { let Latency = 5; }
//def A9WriteFMulD  : SchedWriteRes<[A9UnitFP, A9UnitAGU]> { let Latency = 6; }
//def A9WriteFMAS   : SchedWriteRes<[A9UnitFP, A9UnitAGU]> { let Latency = 8; }

//// A9WriteLM variants expand into a pair of writes for each 64-bit
//// value loaded. When the number of registers is odd, the last
//// A9WriteLnHi is naturally ignored because the instruction has no
//// following def operands.  These variants take no issue resource, so
//// they may need to be part of a WriteSequence that includes A9WriteIssue.
//def A9WriteLM : SchedWriteVariant<[
//  SchedVar<A9LMAdr1Pred, A9WriteLMOpsList.Writes[0-1]>,
//  SchedVar<A9LMAdr2Pred, A9WriteLMOpsList.Writes[0-3]>,
//  SchedVar<A9LMAdr3Pred, A9WriteLMOpsList.Writes[0-5]>,
//  SchedVar<A9LMAdr4Pred, A9WriteLMOpsList.Writes[0-7]>,
//  SchedVar<A9LMAdr5Pred, A9WriteLMOpsList.Writes[0-9]>,
//  SchedVar<A9LMAdr6Pred, A9WriteLMOpsList.Writes[0-11]>,
//  SchedVar<A9LMAdr7Pred, A9WriteLMOpsList.Writes[0-13]>,
//  SchedVar<A9LMAdr8Pred, A9WriteLMOpsList.Writes[0-15]>,
//  // For unknown LDMs, define the maximum number of writes, but only
//  // make the first two consume resources.
//  SchedVar<A9LMUnknownPred, [A9WriteL1, A9WriteL1Hi,
//                             A9WriteL2, A9WriteL2Hi,
//                             A9WriteL3Hi, A9WriteL3Hi,
//                             A9WriteL4Hi, A9WriteL4Hi,
//                             A9WriteL5Hi, A9WriteL5Hi,
//                             A9WriteL6Hi, A9WriteL6Hi,
//                             A9WriteL7Hi, A9WriteL7Hi,
//                             A9WriteL8Hi, A9WriteL8Hi]>]> {
//  let Variadic = 1;
//}
//
////===----------------------------------------------------------------------===//
//// VFP Load/Store Multiple Variants, and NEON VLDn/VSTn support.
//
//// A9WriteLfpOp is the same as A9WriteLSfp but takes no issue resources
//// so can be used in WriteSequences for in single-issue instructions that
//// encapsulate multiple loads.
//def A9WriteLfpOp : SchedWriteRes<[A9UnitLS, A9UnitFP]> {
//  let Latency = 1;
//  let NumMicroOps = 0;
//}
//
//foreach NumAddr = 1-8 in {
//
//// Helper for A9WriteLfp1-8: A sequence of fp loads with no micro-ops.
//def A9WriteLfp#NumAddr#Seq : WriteSequence<[A9WriteLfpOp], NumAddr>;
//
//// A9WriteLfp1-8 definitions are statically expanded into a sequence of
//// A9WriteLfpOps with additive latency that takes a single issue slot.
//// Used directly to describe NEON VLDn.
//def A9WriteLfp#NumAddr : WriteSequence<
//  [A9WriteIssue, !cast<SchedWrite>("A9WriteLfp"#NumAddr#Seq)]>;
//
//// A9WriteLfp1-8Mov adds a cycle of latency and FP resource for
//// permuting loaded values.
//def A9WriteLfp#NumAddr#Mov : WriteSequence<
//  [A9WriteF, !cast<SchedWrite>("A9WriteLfp"#NumAddr#Seq)]>;
//
//} // foreach NumAddr
//
//// Define VLDM/VSTM PreRA resources.
//// A9WriteLMfpPreRA are dynamically expanded into the correct
//// A9WriteLfp1-8 sequence based on a predicate. This supports the
//// preRA VLDM variants in which all 64-bit loads are written to the
//// same tuple of either single or double precision registers.
//def A9WriteLMfpPreRA : SchedWriteVariant<[
//  SchedVar<A9LMAdr1Pred, [A9WriteLfp1]>,
//  SchedVar<A9LMAdr2Pred, [A9WriteLfp2]>,
//  SchedVar<A9LMAdr3Pred, [A9WriteLfp3]>,
//  SchedVar<A9LMAdr4Pred, [A9WriteLfp4]>,
//  SchedVar<A9LMAdr5Pred, [A9WriteLfp5]>,
//  SchedVar<A9LMAdr6Pred, [A9WriteLfp6]>,
//  SchedVar<A9LMAdr7Pred, [A9WriteLfp7]>,
//  SchedVar<A9LMAdr8Pred, [A9WriteLfp8]>,
//  // For unknown VLDM/VSTM PreRA, assume 2xS registers.
//  SchedVar<A9LMUnknownPred, [A9WriteLfp2]>]>;
//
//// Define VLDM/VSTM PostRA Resources.
//// A9WriteLMfpLo takes a LS and FP resource and one issue slot but no latency.
//def A9WriteLMfpLo : SchedWriteRes<[A9UnitLS, A9UnitFP]> { let Latency = 0; }
//
//foreach NumAddr = 1-8 in {
//
//// Each A9WriteL#N variant adds N cycles of latency without consuming
//// additional resources.
//def A9WriteLMfp#NumAddr : WriteSequence<
//  [A9WriteLMfpLo, !cast<SchedWrite>("A9WriteCycle"#NumAddr)]>;
//
//// Assuming aligned access, the upper half of each pair is free with
//// the same latency.
//def A9WriteLMfp#NumAddr#Hi : WriteSequence<
//  [A9WriteLMHi, !cast<SchedWrite>("A9WriteCycle"#NumAddr)]>;
//
//} // foreach NumAddr
//
//// VLDM PostRA Variants. These variants expand A9WriteLMfpPostRA into a
//// pair of writes for each 64-bit data loaded. When the number of
//// registers is odd, the last WriteLMfpnHi is naturally ignored because
//// the instruction has no following def operands.
//
//def A9WriteLMfpPostRAOpsList : A9WriteLMOpsListType<
//                 [A9WriteLMfp1, A9WriteLMfp2,       // 0-1
//                  A9WriteLMfp3, A9WriteLMfp4,       // 2-3
//                  A9WriteLMfp5, A9WriteLMfp6,       // 4-5
//                  A9WriteLMfp7, A9WriteLMfp8,       // 6-7
//                  A9WriteLMfp1Hi,                   // 8-8
//                  A9WriteLMfp2Hi, A9WriteLMfp2Hi,   // 9-10
//                  A9WriteLMfp3Hi, A9WriteLMfp3Hi,   // 11-12
//                  A9WriteLMfp4Hi, A9WriteLMfp4Hi,   // 13-14
//                  A9WriteLMfp5Hi, A9WriteLMfp5Hi,   // 15-16
//                  A9WriteLMfp6Hi, A9WriteLMfp6Hi,   // 17-18
//                  A9WriteLMfp7Hi, A9WriteLMfp7Hi,   // 19-20
//                  A9WriteLMfp8Hi, A9WriteLMfp8Hi]>; // 21-22
//
//def A9WriteLMfpPostRA : SchedWriteVariant<[
//  SchedVar<A9LMAdr1Pred, A9WriteLMfpPostRAOpsList.Writes[0-0, 8-8]>,
//  SchedVar<A9LMAdr2Pred, A9WriteLMfpPostRAOpsList.Writes[0-1, 9-10]>,
//  SchedVar<A9LMAdr3Pred, A9WriteLMfpPostRAOpsList.Writes[0-2, 10-12]>,
//  SchedVar<A9LMAdr4Pred, A9WriteLMfpPostRAOpsList.Writes[0-3, 11-14]>,
//  SchedVar<A9LMAdr5Pred, A9WriteLMfpPostRAOpsList.Writes[0-4, 12-16]>,
//  SchedVar<A9LMAdr6Pred, A9WriteLMfpPostRAOpsList.Writes[0-5, 13-18]>,
//  SchedVar<A9LMAdr7Pred, A9WriteLMfpPostRAOpsList.Writes[0-6, 14-20]>,
//  SchedVar<A9LMAdr8Pred, A9WriteLMfpPostRAOpsList.Writes[0-7, 15-22]>,
//  // For unknown LDMs, define the maximum number of writes, but only
//  // make the first two consume resources. We are optimizing for the case
//  // where the operands are DPRs, and this determines the first eight
//  // types. The remaining eight types are filled to cover the case
//  // where the operands are SPRs.
//  SchedVar<A9LMUnknownPred, [A9WriteLMfp1, A9WriteLMfp2,
//                             A9WriteLMfp3Hi, A9WriteLMfp4Hi,
//                             A9WriteLMfp5Hi, A9WriteLMfp6Hi,
//                             A9WriteLMfp7Hi, A9WriteLMfp8Hi,
//                             A9WriteLMfp5Hi, A9WriteLMfp5Hi,
//                             A9WriteLMfp6Hi, A9WriteLMfp6Hi,
//                             A9WriteLMfp7Hi, A9WriteLMfp7Hi,
//                             A9WriteLMfp8Hi, A9WriteLMfp8Hi]>]> {
//  let Variadic = 1;
//}
//
//// Distinguish between our multiple MI-level forms of the same
//// VLDM/VSTM instructions.
//def A9PreRA : SchedPredicate<
//  "TargetRegisterInfo::isVirtualRegister(MI->getOperand(0).getReg())">;
//def A9PostRA : SchedPredicate<
//  "TargetRegisterInfo::isPhysicalRegister(MI->getOperand(0).getReg())">;
//
//// VLDM represents all destination registers as a single register
//// tuple, unlike LDM. So the number of write operands is not variadic.
//def A9WriteLMfp : SchedWriteVariant<[
//  SchedVar<A9PreRA, [A9WriteLMfpPreRA]>,
//  SchedVar<A9PostRA, [A9WriteLMfpPostRA]>]>;
//
////===----------------------------------------------------------------------===//
//// Resources for other (non-LDM/VLDM) Variants.
//
//// These mov immediate writers are unconditionally expanded with
//// additive latency.
//def A9WriteI2 : WriteSequence<[A9WriteI, A9WriteI]>;
//def A9WriteI2pc : WriteSequence<[A9WriteI, A9WriteI, WriteALU]>;
//def A9WriteI2ld  : WriteSequence<[A9WriteI, A9WriteI, A9WriteL]>;
//
//// Some ALU operations can read loaded integer values one cycle early.
//def A9ReadALU : SchedReadAdvance<1,
//  [A9WriteL, A9WriteLHi, A9WriteLsi, A9WriteLb, A9WriteLbsi,
//   A9WriteL1, A9WriteL2, A9WriteL3, A9WriteL4,
//   A9WriteL5, A9WriteL6, A9WriteL7, A9WriteL8,
//   A9WriteL1Hi, A9WriteL2Hi, A9WriteL3Hi, A9WriteL4Hi,
//   A9WriteL5Hi, A9WriteL6Hi, A9WriteL7Hi, A9WriteL8Hi]>;

}
