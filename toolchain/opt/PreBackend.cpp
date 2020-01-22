#include <llvm/Pass.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>
#include <algorithm>
#include <vector>
#include <map>

#include "Common.hpp"

using namespace llvm;

namespace {

bool isUsedBy(std::vector<Value const*>& visitedValues, Value const* value,
    User const* expected_user) {
  visitedValues.push_back(value);
  for(User const* user : value->users()) {
    if(Instruction const* instr = dyn_cast<Instruction>(user)) {
      BasicBlock const* block = instr->getParent();
      user = block->getParent();
    }
    if(std::find(visitedValues.begin(), visitedValues.end(),
          dynamic_cast<Value const*>(user)) != visitedValues.end())
      continue;
    if(user == expected_user)
      return true;
    if(isUsedBy(visitedValues, user, expected_user))
      return true;
  }
  return false;
}

bool isUsedBy(Value const* value, User const* expected_user) {
  std::vector<Value const*> visitedValues;
  return isUsedBy(visitedValues, value, expected_user);
}

class ChipsOpt : public ModulePass {
private:
  std::ofstream jsonOut;

  using ValuesToIoIds = std::map<GlobalVariable const*, int>;

public:
  static char ID;
  ChipsOpt() : ModulePass(ID) { }

  bool doInitialization(Module& module) override {
    jsonOut.open("out.opt.json", std::ofstream::binary | std::ofstream::trunc);
    return false;
  }
  bool doFinalization(Module& module) override {
    jsonOut.close();
    return false;
  }

  int getIoId(ValuesToIoIds ids, Value const* op) {
    GlobalVariable const* global_var = dyn_cast<GlobalVariable>(op);
    if(!global_var)
      return -1;
    //errs() << "relevant var " << global_var->getName() << " found\n";
    auto iter = ids.find(global_var);
    if(iter == ids.end())
      return -1;
    return iter->second;
  }

  bool is_input(std::string string) {
    size_t space_pos = string.find(" ");
    return string.substr(0, space_pos) == "input";
  }

  void findIoIdentifiers(Module const& module, ValuesToIoIds& ids) {
    jsonOut << "\"io_ids\": [";
    bool firstElement = true;
    int input_id = 0;
    int output_id = 0;

    for(GlobalVariable const& global : module.globals()) {
      if(global.getName() != std::string("llvm.global.annotations"))
        continue;
      Constant const* value = global.getInitializer();
      ConstantArray const* array = cast<ConstantArray>(value);
      for(unsigned i = 0; i < array->getNumOperands(); ++i) {
        ConstantStruct const* element = cast<ConstantStruct>(
            array->getOperand(i));
        GlobalValue const* target = cast<GlobalValue>(
            element->getOperand(0)->stripPointerCasts());
        GlobalVariable const* string_constant = dyn_cast<GlobalVariable>(
            element->getOperand(1)->stripPointerCasts());
        StringRef string = dyn_cast<ConstantDataArray>(
            string_constant->getInitializer())->getAsString();

        if(!target->getType()->getElementType()->isIntegerTy(32))
          continue;

        std::string std_string = string.str();

        int id = input_id;
        if(is_input(std_string)) {
          input_id += 1;
        } else {
          id = output_id;
          output_id += 1;
        }

        ids[cast<GlobalVariable>(target)] = id;

        if(!firstElement)
          jsonOut << ", ";
        jsonOut << "[" << id << ", ";
        jsonOut << "\"" << std_string.substr(0, std_string.size() - 1) << "\"";
        jsonOut << "]";
        firstElement = false;
      }
    }
    jsonOut << "]";
  }

  bool runOnModule(Module& module) override {
    jsonOut << "{";

    ValuesToIoIds valuesToIoIds;
    findIoIdentifiers(module, valuesToIoIds);

    jsonOut << "}";

    for(Function& func : module.functions()) {
      //func.viewCFG();
      std::map<Instruction*, int> constant_instrs;
      for(Instruction& instr : instructions(func)) {
        if(instr.getOpcode() == Instruction::Load) {
          for(Use const& op : instr.operands()) {
            int ioId = getIoId(valuesToIoIds, op);
            if(ioId != -1) {
              constant_instrs.emplace(&instr, ioId);
              break;
            }
          }
        }

        for(size_t i = 0; i < instr.getNumOperands(); ++i) {
          Value* op = instr.getOperand(i);
          Instruction* referenced_instr = dyn_cast<Instruction>(op);
          if(!referenced_instr)
            continue;
          auto iter = constant_instrs.find(referenced_instr);
          if(iter == constant_instrs.end())
            continue;
          int ioId = iter->second;
//          errs() << "replacing with " << ioId << " in ";
//          instr.print(errs());
//          errs() << "\n";
          instr.setOperand(i, Constant::getIntegerValue(
                IntegerType::get(func.getContext(), 32),
                APInt(32, ioId)));
        }
      }

      for(auto& constant_instr : constant_instrs)
        constant_instr.first->eraseFromParent();
    }

    Function* mainFunc = findMainFunction(module);
    if(!mainFunc) {
      errs() << "no main function found\n";
      return true;
    }
    mainFunc->setName("main");

    std::vector<Value*> globalValues;
    for(GlobalVariable& global : module.globals()) {
      if(global.getName().find("llvm.") == 0)
        continue;
      //To prevent an error message with internal linkage globals
//      global.setDSOLocal(true);
      globalValues.push_back(&global);
    }
    for(Function& func : module.functions()) {
      if(&func == mainFunc)
        continue;
//      func.setDSOLocal(true);
      globalValues.push_back(&func);
    }

    std::vector<Value*> toRemove;
    for(Value* value : globalValues) {
      if(!isUsedBy(value, mainFunc)) {
        //value->dropAllReferences();
        toRemove.push_back(value);
      }
    }

    GlobalVariable* annotations = module.getGlobalVariable(
        "llvm.global.annotations");
    if(annotations)
      annotations->eraseFromParent();
    for(Value* value : toRemove) {
      Function* func = dyn_cast<Function>(value);
      GlobalVariable* var = dyn_cast<GlobalVariable>(value);
      //errs() << "removing " << value->getName() << "\n";
      if(func)
        func->eraseFromParent();
      else if(var)
        var->eraseFromParent();
    }

    //errs() << "done erasing\n";
    
    //module.dump();
    //mainFunc->viewCFG();
    //module.print(errs(), true);

//    std::vector<GlobalVariable*> toRemove;
//    for(GlobalVariable& global : module.globals()) {
//      if(global.getName() != std::string("llvm.global.annotations"))
//        continue;
//      global.dropAllReferences();
//      toRemove.push_back(&global);
//    }
//    for(GlobalVariable* var : toRemove) {
//      errs() << "removing " << var->getName() << "\n";
//      var->eraseFromParent();
//    }
//
//    std::vector<Function*> funcsToRemove;
//    for(Function& func : module.functions()) {
//      if(func.getName() == "fgets") {
//        errs() << "fgets: " << func.getNumUses() << "\n";
//        errs() << "use: ";
//        func.user_begin()->print(errs());
//        errs() << "\n";
//        errs() << "use is same: " << (func.use_begin()->getUser() == &dynamic_cast<User&>(func)) << "\n";
//      }
//      if(!func.use_empty() || func.getName() == "main")
//        continue;
//      funcsToRemove.push_back(&func);
//    }
//    for(Function* func : funcsToRemove) {
//      errs() << "removing " << func->getName() << "\n";
//      func->eraseFromParent();
//    }
//
//    std::vector<GlobalVariable*> globalsToRemove;
//    for(GlobalVariable& global : module.globals()) {
////      errs() << "checking " << global.getName() << "\n";
////      if(!global.hasOneUse()) {
////        errs() << "uses of ";
////        global.print(errs());
////        errs() << ":\n";
////        for(Use const& use : global.uses()) {
////          use->print(errs());
////          errs() << "\n";
////        }
////      }
//      //A value always uses itself
//      if(!global.hasOneUse() || global.getName().find("llvm.") == 0)
//        continue;
//      globalsToRemove.push_back(&global);
//    }
//    for(GlobalVariable* global : globalsToRemove) {
//      errs() << "removing " << global->getName() << "\n";
//      global->eraseFromParent();
//    }

    //TODO maybe run optimization passes explicitly (-simplifycfg)
    //see https://llvm.org/docs/Passes.html
    return false;
  }
};
char ChipsOpt::ID = 0;
}

struct Test {
  Test() {
  }
};

Test x;

static RegisterPass<ChipsOpt> X("chipsopt",
    "Chips optimization and analysis pass",
    /*CFGOnly=*/false, /*is_analysis=*/false);
