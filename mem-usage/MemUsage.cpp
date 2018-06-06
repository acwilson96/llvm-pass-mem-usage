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
            
            errs() << "\nLooking at Global Variables" << "\n";
            errs() << "\e[1m" << "\tVariable Name\t\t\tSize (Bits)" << "\e[0m" << "\n";
            for (GlobalVariable &Global : M.getGlobalList()) {
                string gVarName = Global.getName();
                if (Global.getValueType()->isArrayTy()) {
                    errs() << "\t" << gVarName << getTabs(gVarName);
                    errs() << (Global.getValueType()->getArrayElementType()->getScalarSizeInBits() * Global.getValueType()->getArrayNumElements()) << "\t- " << *Global.getValueType();
                }
                else {
                    errs() << "\t" << gVarName << getTabs(gVarName) << Global.getValueType()->getScalarSizeInBits() << "\t- " << *Global.getValueType();
                }
                errs() << "\n";
            }

            errs() << "\nLooking at Functions:" << "\n";
            for (Function &Function: M.getFunctionList()) {
                errs() << "\t - " << Function.getName() << "\n"; 
            }

            return false;
        }

        virtual int runOnFunction(Function &F) {
            errs() << "\tIn a Function called " << F.getName() << "\n";
            return 0;
        }

        string getTabs(const string &varName) {
            // 16 Characters between.
            int num = 4 - varName.length() % 4;
            string output = "";
            for (int i=0; i < num; i++)
                output += "\t";
            return output;
        }

    };
    
}
char MemoryUsage::ID = 0;
static RegisterPass<MemoryUsage> X("mem-usage", "Simple Memory Usage Estimator");