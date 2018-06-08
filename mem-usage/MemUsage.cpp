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
            errs() << "In a Module called " << M.getName() << "\n";

            int totalUsage      = 0;
            totalUsage += getGlobalsUsage(M);
            totalUsage += getFunctionsUsage(M);

            return false;
        }

        int getGlobalsUsage(Module &M) {
            int output = 0;
            errs() << "\nLooking at Global Variables" << "\n\n";
            errs() << "\tVariable Name\t\t\tSize (Bits)" << "\n";
            errs() << "\t-------------------------------------------" << "\n";
            for (GlobalVariable &Global : M.getGlobalList()) {
                string gVarName = Global.getName();
                if (Global.getValueType()->isArrayTy()) {
                    int usage = (Global.getValueType()->getArrayElementType()->getScalarSizeInBits() * Global.getValueType()->getArrayNumElements());
                    errs() << "\t" << gVarName << getTabs(gVarName) << usage << "\t- " << *Global.getValueType();
                    output += usage;
                }
                else {
                    int usage = Global.getValueType()->getScalarSizeInBits();
                    errs() << "\t" << gVarName << getTabs(gVarName) << usage << "\t- " << *Global.getValueType();
                    output += usage;
                }
                errs() << "\n";
            }
            errs() << "\e[1m" << "\nGlobal Usage: " << output << " Bits\n" <<  "\e[0m";
            return output;
        }

        int getFunctionsUsage(Module &M) {
            int output = 0;
            errs() << "\nLooking at Functions:" << "\n";
            for (Function &F: M.getFunctionList()) {
                errs() << "\t" << F.getName() << ":\n";
                output += getFunctionUsage(F);
            }
            return output;
        }

        int getFunctionUsage(Function &F) {
            int output = 0;

            errs() << "\tVariable Name\t\t\tSize (Bits)" << "\n";
            errs() << "\t-------------------------------------------" << "\n";

            for (auto bb = F.getBasicBlockList().rbegin(), e = F.getBasicBlockList().rend(); bb != e; ++bb) {
                for (BasicBlock::reverse_iterator i = bb->rbegin(), e = bb->rend(); i != e; ++i) {
                    // Declare all members.
                    Instruction *currInst= &*i;
                    output += getInstUsage(currInst);
                }
            }

            return output;
        }

        int getInstUsage(Instruction* i) {
            int output = 0;

            return output;
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