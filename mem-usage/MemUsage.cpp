#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <set>



using namespace llvm;
using namespace std;
namespace {

    struct MemoryUsage : public ModulePass {

        DenseMap<Function*, SmallVector<Function*,64>> functionCalls;
        DenseMap<Function*, set<Instruction*>> functionVars;

        static char ID;
        MemoryUsage() : ModulePass(ID) {}
        virtual bool runOnModule(Module &M) {
            errs() << "In a Module called " << M.getName() << "\n";

            int totalUsage      = 0;
            totalUsage += getGlobalsUsage(M);
            totalUsage += getFunctionsUsage(M);

            errs() << "\e[1m" << "\n\nTOTAL STATIC USAGE: " << totalUsage << " Bits\n\n";

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
            errs() << "\nLooking at Functions:";
            for (Function &F: M.getFunctionList()) {
                SmallVector<Function*,64> currFunctionCalls;
                functionCalls[&F] = currFunctionCalls;
                errs() << "\n\n" << F.getName() << ":";
                output += getFunctionUsage(F);
            }
            return output;
        }

        int getFunctionUsage(Function &F) {
            int output = 0;

            // Populate Data on Function.
            for (auto bb = F.getBasicBlockList().rbegin(), e = F.getBasicBlockList().rend(); bb != e; ++bb) {
                for (BasicBlock::reverse_iterator i = bb->rbegin(), e = bb->rend(); i != e; ++i) {
                    Instruction *currInst= &*i;

                    // If Instruction uses memory, store it.
                    if (currInst->getType()->getScalarSizeInBits() > 0)
                        functionVars[&F].insert(currInst);

                    // If Instruction is Function Call, store it.
                    if (isa<CallInst>(currInst))
                        functionCalls[&F].push_back(cast<CallInst>(currInst)->getCalledFunction());
                }
            }

            // Sum & Output Variable Allocations.
            errs() << "\n\tVariable Name\t\t\tSize (Bits)" << "\n";
            errs() << "\t-------------------------------------------";
            for (Instruction* v: functionVars[&F]) {
                int usage   = v->getType()->getScalarSizeInBits();
                string name = v->getName();
                if (name == "") name = "....";
                errs() << "\n\t" << name << getTabs(name) << usage << "\t- " << *(v->getType());
                output += usage;
            }
            errs() << "\n\t-------------------------------------------";
            errs() << "\e[1m" << "\n\t\t\t\t\t" << output << " Bits \e[0m" << "\n";


            // Output Function Calls.
            errs() << "\n\tCalls Functions:" << "\n";
            errs() << "\t-------------------------------------------";
            for (Function* f: functionCalls[&F])
                errs() << "\n\t" << f->getName() << "()";
            errs() << "\n\t-------------------------------------------";


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
            int num = 4 - (varName.length() % 4);
            string output = "";
            for (int i=0; i < num; i++)
                output += "\t";
            if (varName.length() == 1) output += "\t";
            if (varName.length() == 2) output += "\t\t";
            if (varName.length() == 3) output += "\t\t\t";
            return output;
        }

    };
    
}

char MemoryUsage::ID = 0;
static RegisterPass<MemoryUsage> X("mem-usage", "Simple Memory Usage Estimator");