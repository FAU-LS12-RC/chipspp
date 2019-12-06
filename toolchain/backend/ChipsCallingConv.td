def CC_Chips : CallingConv<[
  CCIfType<[i32, f32], CCAssignToReg<[R0, R1, R2, R4]>>,
  CCAssignToStack<4, 4>
]>;

def RetCC_Chips : CallingConv<[
  CCIfType<[f64], CCAssignToReg<[RA]>>,
  CCAssignToReg<[R0, R1]>
]>;

def CSR : CalleeSavedRegs<(add R6)> {
//  let OtherPreserved = (add (sequence "I%u", 0, 7),
//                            (sequence "L%u", 0, 7));
}
