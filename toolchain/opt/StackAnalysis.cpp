#include <llvm/Pass.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/CallGraph.h>

#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <set>

#include "Common.hpp"

using namespace llvm;

namespace {

int const DONE = -1;

class ChipsStackAnalysis : public ModulePass {
private:
  using StackSizesMap = std::unordered_map<std::string, size_t>;
  using VisitedNodesSet = std::unordered_map<CallGraphNode const*, int>;
  std::ofstream jsonOut;
  StackSizesMap stackSizes;

public:
  static char ID;
  ChipsStackAnalysis() : ModulePass(ID) { }

  bool doInitialization(Module& module) override {
    jsonOut.open("out.stack-analysis.json",
        std::ofstream::binary | std::ofstream::trunc);

    std::ifstream stackSizesFile("out.backend.stack-sizes",
        std::ifstream::binary);
    assert(!stackSizesFile.fail() && "could not open stack sizes file");

    for(std::string line; std::getline(stackSizesFile, line);) {
      size_t delimiterPos = line.find(" ");
      assert(delimiterPos != std::string::npos);
      std::string funcName(line.substr(0, delimiterPos));
      size_t stackSize = std::stoi(line.substr(delimiterPos + 1));
      stackSizes.insert({ funcName, stackSize });
    }

//    for(auto& s : stackSizes)
//      errs() << "func ‘" << s.first << "’: " << s.second << "\n";

    return false;
  }
  bool doFinalization(Module& module) override {
    jsonOut.close();
    return false;
  }

  void getAnalysisUsage(AnalysisUsage& au) const override {
    au.addRequired<CallGraphWrapperPass>();
  }

  bool isBackEdgeChild(VisitedNodesSet const& visitedNodes,
      CallGraphNode const* child, int parentId) {
    auto childVisitedRecord = visitedNodes.find(child);
    if(childVisitedRecord != visitedNodes.end()) {
      size_t childId = childVisitedRecord->second;
      if(childId != DONE && childId <= parentId)
        return true;
    }
    return false;
  }

  int getOrInsertNodeId(VisitedNodesSet& visitedNodes, int& nodeIdCounter,
      CallGraphNode const* node) {
    auto visitedRecord = visitedNodes.find(node);
    if(visitedRecord == visitedNodes.end()) {
      size_t ret = nodeIdCounter;
      visitedNodes.insert({ node, ret });
      nodeIdCounter += 1;
      return ret;
    }
    return visitedRecord->second;
  }

  bool findLongestPathLengthToLeaf(CallGraphNode const* node,
      VisitedNodesSet& visitedNodes, int& nodeIdCounter,
      StackSizesMap const& stackSizes, size_t& max, size_t pathLength) {
    int nodeId = getOrInsertNodeId(visitedNodes, nodeIdCounter, node);
//    errs() << "-- visiting call to " << node->getFunction()->getName() << 
//      " (" << nodeId << ")" << "\n";

    size_t numberOfCalledFuncs = 0;

    for(auto const& call : *node) {
      CallGraphNode const* child = call.second;
      Function* calledFunc = child->getFunction();
      if(!calledFunc)
        continue;
      numberOfCalledFuncs += 1;

      if(calledFunc->isVarArg()) {
        errs() << "stack-analysis: vararg function found\n";
        return false;
      }
      auto callWeightItem = stackSizes.find(calledFunc->getName().str());
      if(callWeightItem == stackSizes.end())
        errs() << "function not found: " << calledFunc->getName().str() << "\n";
      size_t callWeight = stackSizes.at(calledFunc->getName().str());

      if(isBackEdgeChild(visitedNodes, child, nodeId)) {
        errs() << "stack-analysis: recursion detected\n";
        return false;
      }

      if(!findLongestPathLengthToLeaf(
            child, visitedNodes, nodeIdCounter, stackSizes, max,
            pathLength + callWeight))
        return false;
    }

    visitedNodes[node] = DONE;

    bool isLeaf = numberOfCalledFuncs == 0;
    if(isLeaf) {
      if(pathLength > max)
        max = pathLength;
      return true;
    }

    return true;
  }

  bool runOnModule(Module& module) override {
    CallGraph& callGraph = getAnalysis<CallGraphWrapperPass>().getCallGraph();
//    callGraph.print(errs());

    Function* mainFunc = findMainFunction(module);
    assert(mainFunc && "No main function found.");
    size_t totalStackSize = 0;
    int nodeId = 0;
    VisitedNodesSet visitedNodes;
    bool found = findLongestPathLengthToLeaf(callGraph[mainFunc],
        visitedNodes, nodeId, stackSizes, totalStackSize,
        stackSizes.at(mainFunc->getName().str()));
    jsonOut << "{ ";
    if(found)
      jsonOut << "\"stack_size\": " << totalStackSize ;
    jsonOut << " }\n";

    return false;
  }
};
char ChipsStackAnalysis::ID = 0;
}

static RegisterPass<ChipsStackAnalysis> X("chipsstack",
    "Chips stack analysis pass (based on call graph)",
    /*CFGOnly=*/false, /*is_analysis=*/true);
