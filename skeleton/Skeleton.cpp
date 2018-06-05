#define DEBUG_TYPE "skeletonpass"
#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>
using namespace llvm;
using namespace std;
namespace {

    struct MemoryUsage : public FunctionPass {
        
        static char ID;
        MemoryUsage() : FunctionPass(ID) {}
        virtual bool runOnFunction(Function &F) {
            errs() << F.getName() << "\n";
            return false;
        }

    };
    
}
char MemoryUsage::ID = 0;
static RegisterPass<MemoryUsage> X("mem-usage", "Simple Memory Usage Estimator");