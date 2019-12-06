include "llvm/Target/Target.td"

include "ChipsRegisterInfo.td"
include "ChipsCallingConv.td"
include "ChipsSchedule.td"
include "ChipsInstrInfo.td"

def ChipsInstrInfo : InstrInfo;

def ChipsAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  int PassSubtarget = 1;
  int Variant = 0;
}

def Chips : Target {
  // Pull in Instruction Info:
  let InstructionSet = ChipsInstrInfo;
//  let AssemblyParsers  = [SparcAsmParser];
  let AssemblyWriters = [ChipsAsmWriter];
//  let AllowRegisterRenaming = 1;
}

class Proc<string Name, list<SubtargetFeature> Features>
 : ProcessorModel<Name, ChipsGenericModel, Features>;
def : Proc<"chips", []>;
