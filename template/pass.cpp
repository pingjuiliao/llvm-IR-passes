#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

// Project Name : PrintFunc
//   (replace this with your NewProjectName )

namespace {
    struct PrintFuncPass : public FunctionPass {
        static char ID ;
        PrintFuncPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            errs() << "I saw a function called " << F.getName() << "!\n" ;
            return false ;
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
