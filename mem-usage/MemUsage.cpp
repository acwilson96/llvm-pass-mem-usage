#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace llvm;
using namespace std;
namespace {

    struct MemoryUsage : public ModulePass {

        static char ID;
        MemoryUsage() : ModulePass(ID) {}
        virtual bool runOnModule(Module &M) {
            SmallVector<Value*, 64> globalVariables;

            errs() << "In a Module called " << M.getName() << "\n";
            
            errs() << "Looking at Global Variables" << "\n";
            errs() << "\e[1m" << "\tName\t\tSize" << "\e[0m" << "\n";
            for (GlobalVariable &Global : M.getGlobalList()) {
                errs() << "\t" << Global.getName() << "\t\t" << *Global.getValueType() << "\n";
            }

            errs() << "Looking at Functions:" << "\n";
            for (Function &Function: M.getFunctionList()) {
                errs() << "\t - " << Function.getName() << "\n"; 
            }

            return false;
        }


        virtual int runOnFunction(Function &F) {
            errs() << "\tIn a Function called " << F.getName() << "\n";
            return 0;
        }

    };
    
}
char MemoryUsage::ID = 0;
static RegisterPass<MemoryUsage> X("mem-usage", "Simple Memory Usage Estimator");