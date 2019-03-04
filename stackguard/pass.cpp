#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstIterator.h"
using namespace llvm;

namespace {
    struct PrintFuncPass : public FunctionPass {
        static char ID ;
        PrintFuncPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
                errs() << *I << "\n";
            return true ;
        }
    };
}

char PrintFuncPass::ID = 0;

static void
registerPrintFuncPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new PrintFuncPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerPrintFuncPass);
