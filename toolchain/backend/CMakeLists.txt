cmake_minimum_required(VERSION 3.10)
project(Codegen)

#set(CXXFLAGS -std=c++14)
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

set(LLVM_TABLEGEN_EXE llvm-tblgen)

find_package(LLVM CONFIG REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
set(LLVM_MAIN_INCLUDE_DIR ${LLVM_INCLUDE_DIR})

#set(LLVM_TABLEGEN_EXE $ENV{SCR}/llvm-6-install/bin/llvm-tblgen)
#set(LLVM_MAIN_INCLUDE_DIR $ENV{SCR}/llvm-6-install/include)
#list(APPEND CMAKE_MODULE_PATH $ENV{SCR}/llvm-6-install/lib/cmake/llvm)

include(AddLLVM)
include(TableGen)

include_directories(BEFORE "${LLVM_MAIN_INCLUDE_DIR}")

message(STATUS "llvm_tablegen_flags ${LLVM_TABLEGEN_FLAGS}")

set(LLVM_TARGET_DEFINITIONS Chips.td)

tablegen(LLVM ChipsGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ChipsGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ChipsGenCallingConv.inc -gen-callingconv)
tablegen(LLVM ChipsGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ChipsGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ChipsGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(ChipsCommonTableGen)

add_llvm_target(ChipsCodeGen
  main.cpp SHARED)
