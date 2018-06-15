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

        Function* main;
        DenseMap<GlobalVariable*, int> globalsUsage;
        DenseMap<Function*, set<Instruction*>> functionUsage;
        DenseMap<Function*, SmallVector<Function*,64>> functionCalls;

        static char ID;
        MemoryUsage() : ModulePass(ID) {}
        virtual bool runOnModule(Module &M) {
            errs() << "Performing Static Analysis on Module: " << M.getName() << "\n";

            parseModule(M);
            int staticUsage = getStaticUsage();
            errs() << "\e[1m" << "\n\nTotal Static Usage:\t" << staticUsage << "\tBits\n\n" << "\e[0m";

            /* Detect Recursion */
            bool recursiveModule = false;
            for (auto mapEntry: functionCalls) {
                SmallVector<Function*,64> callStack;
                recursiveModule = recursiveModule || isRecursive(mapEntry.first, callStack);
            }

            if (recursiveModule)
                errs() << "\nModule Contains Recursive Code - Analysis Finished";
            else {
                errs() << "\nAnalysing Max Memory Usage...\n";
                int maxUsage = getGlobalsUsage();
                maxUsage    += visitFunction(main);
                errs() << "\nMax Memory Used:\t" << maxUsage << "\tBits";
            }

            // Debugging
            errs() << "\n\n ---- DEBUGGING ---\n";

            for (auto funcPair: functionUsage) {
                errs() << "\n\n" << *funcPair.first;
            }

            errs() << "\n";
            return false;
        }

        /* Dynamic Analysis */


        int visitFunction(Function* f) {
            int funcUsage = getFunctionUsage(f);
            errs() << f->getName();
            for (Function* call: functionCalls[f]) {
                errs() << "->";
                funcUsage += visitFunction(call);
                errs() << "\n";
            }
            return funcUsage;
        }

        /* Static Analysis */

        /* Parses the Module Populating Compiler Data Structures */
        void parseModule(Module &M) {
            /* Parse Globals */
            for (GlobalVariable &Global : M.getGlobalList()) {
                if (Global.getValueType()->isArrayTy())
                    globalsUsage[&Global] = (Global.getValueType()->getArrayElementType()->getScalarSizeInBits() * Global.getValueType()->getArrayNumElements());
                else
                    globalsUsage[&Global] = Global.getValueType()->getScalarSizeInBits();
            }

            /* Parse Functions */
            for (Function &F: M.getFunctionList()) {
                if (F.getName() == "main")
                    main = &F;
                for (auto bb = F.getBasicBlockList().begin(), e = F.getBasicBlockList().end(); bb != e; ++bb) {
                    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
                        Instruction *currInst= &*i;

                        // If Instruction uses memory, store it.
                        if (currInst->getType()->getScalarSizeInBits() > 0)
                            functionUsage[&F].insert(currInst);

                        // If Instruction is Function Call, store it.
                        if (isa<CallInst>(currInst))
                            functionCalls[&F].push_back(cast<CallInst>(currInst)->getCalledFunction());
                    }
                }
            }
        }

        /* Calculates Static Usage of Current Module in Bits */
        int getStaticUsage() {
            int output = 0;

            output += getGlobalsUsage();
            errs() << "\nGlobal Variables Use: \t" << output << "\tBits";

            output += getFunctionsUsage();

            return output;
        }

        /* Sums Global Variables Usage */
        int getGlobalsUsage() {
            int output = 0;
            for (auto gvPair: globalsUsage) {
                output += gvPair.second;
            }
            return output;
        }

        /* Sums Usage of All Functions */
        int getFunctionsUsage() {
            int output = 0;
            int currFuncUsage = 0;
            errs() << "\nFunctions:";
            for (auto funcPair: functionUsage) {
                currFuncUsage = getFunctionUsage(funcPair.first);
                errs() << "\n\t" << funcPair.first->getName() << " Uses:\t" << currFuncUsage << "\tBits";
                output += currFuncUsage;
            }
            return output;
        }

        /* Sums Usage of a Function */
        int getFunctionUsage(Function* f) {
            int output = 0;
            for (auto var: functionUsage[f]) {
                output += var->getType()->getScalarSizeInBits();
            }
            return output;
        }

        /* Helpers */

        bool isRecursive(Function* func, SmallVector<Function*,64> callStack) {
            bool output = false;
            if (setContains(func, &callStack))
                return true;
            callStack.push_back(func);
            for (Function* call: functionCalls[func]) {
                output = output || isRecursive(call, callStack);
            }
            return output;
        }

        bool setContains(Function *target, SmallVector<Function*, 64> *set) {
            bool output = false;
            for (Function *elem: *set) {
                if (elem == target) output = true;
            }
            return output;
        }

    };
    
}

char MemoryUsage::ID = 0;
static RegisterPass<MemoryUsage> X("mem-usage", "Simple Memory Usage Estimator");