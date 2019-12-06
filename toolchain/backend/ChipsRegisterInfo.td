//===-- SparcRegisterInfo.td - Sparc Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Sparc register file
//===----------------------------------------------------------------------===//

class ChipsReg<string n> : Register<n> {
  let Namespace = "Chips";
}

def R0 : ChipsReg<"r0">;
def R1 : ChipsReg<"r1">;
def R2 : ChipsReg<"r2">;
def R3 : ChipsReg<"r3">;
def R4 : ChipsReg<"r4">;
def R5 : ChipsReg<"r5">;
def R6 : ChipsReg<"r6">;
def R7 : ChipsReg<"r7">;
def R8 : ChipsReg<"r8">;
def R9 : ChipsReg<"r9">;
def R10 : ChipsReg<"r10">;
def R11 : ChipsReg<"r11">;
def R12 : ChipsReg<"r12">;
def R13 : ChipsReg<"r13">;
def R14 : ChipsReg<"r14">;
def R15 : ChipsReg<"r15">;

def Carry : ChipsReg<"carry">;
def RALo : ChipsReg<"a_lo">;
def RAHi : ChipsReg<"a_hi">;
def RBLo : ChipsReg<"b_lo">;
def RBHi : ChipsReg<"b_hi">;

let Namespace = "Chips" in {
  def DoubleLoIndex : SubRegIndex<32, 0>;
  def DoubleHiIndex : SubRegIndex<32, 32>;
}
def RA : ChipsReg<"a"> {
  let CoveredBySubRegs = 1;
  let SubRegs = [RALo, RAHi];
  let SubRegIndices = [DoubleLoIndex, DoubleHiIndex];
}
def RB : ChipsReg<"b"> {
  let CoveredBySubRegs = 1;
  let SubRegs = [RBLo, RBHi];
  let SubRegIndices = [DoubleLoIndex, DoubleHiIndex];
}

class RegPair<string name, Register hi, Register lo> :
  ChipsReg<!strconcat("r", name)> {
  let CoveredBySubRegs = 1;
  let SubRegs = [hi, lo];
  let SubRegIndices = [DoubleLoIndex, DoubleHiIndex];
}

def RP0 : RegPair<"0,1", R0, R1>;
//def RP1 : RegPair<"2,3", R2, R3>;
def RP1 : RegPair<"4,5", R4, R5>;
//def RP3 : RegPair<"6,7", R6, R7>;
def RP2 : RegPair<"8,9", R8, R9>;
def RP3 : RegPair<"10,11", R10, R11>;
//def RP6 : RegPair<"12,13", R12, R13>; //reserved
def RP4 : RegPair<"14,15", R14, R15>;

def AllRegs : RegisterClass<"Chips", [i32, f32], 32,
  (add (sequence "R%u", 0, 15))>; //RA, RB
//def Int64Regs : RegisterClass<"Chips", [i64], 32,
//  (sequence "RP%u", 0, 4)>;
def IntRegs : RegisterClass<"Chips", [i32], 32, (sequence "R%u", 0, 15)>;
def FpRegs : RegisterClass<"Chips", [f32], 32, (sequence "R%u", 0, 15)>;

//let isAllocatable = 1 in
//def DFpRegs : RegisterClass<"Chips", [f64], 64, (add RA, RB, RP0, RP1)>;
def DFpRegs : RegisterClass<"Chips", [f64], 32, (add RA, RB)>;

def DFpSubRegs : RegisterClass<"Chips", [f32], 32,
    (add RALo, RAHi, RBLo, RBHi)>;

def DFpResultRegs : RegisterClass<"Chips", [f64], 32, (add RA)>;

//def DFpRegs : RegisterClass<"Chips", [f64], 32, (sequence "RP%u", 0, 4)>;
