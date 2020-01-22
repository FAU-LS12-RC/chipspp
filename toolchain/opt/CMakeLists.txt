cmake_minimum_required(VERSION 3.10.2)
project(ChipsOpt)

#set(CXXFLAGS -std=c++11)
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(LLVM CONFIG REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

set(LLVM_OPTIONAL_SOURCES PreBackend.cpp StackAnalysis.cpp)

message(STATUS "LLVM_CMAKE_DIR='${LLVM_CMAKE_DIR}'")
include_directories(${LLVM_INCLUDE_DIRS})

#add_llvm_loadable_module(LLVMChipsOpt PreBackend.cpp 
#  PLUGIN_TOOL opt)
#add_llvm_loadable_module(LLVMChipsStackAnalysis StackAnalysis.cpp 
#  PLUGIN_TOOL opt)
add_llvm_library(LLVMChipsOpt PreBackend.cpp 
  MODULE PLUGIN_TOOL opt)
add_llvm_library(LLVMChipsStackAnalysis StackAnalysis.cpp 
  MODULE PLUGIN_TOOL opt)
