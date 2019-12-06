#ifndef COMMON_H
#define COMMON_H

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>

namespace {

llvm::Function* findMainFunction(llvm::Module& module) {
  for(llvm::Function& func : module.functions()) {
    if(func.getName() == "main" || func.getName() == "_Z4mainv")
      return &func;
  }
  return nullptr;
}

}

#endif
