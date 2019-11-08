#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

// Project Name : AflLLVM
//   (replace this with your NewProjectName )

namespace {
    struct AflLLVMPass : public FunctionPass {
        static char ID ;
        AflLLVMPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            errs() << "I saw a function called " << F.getName() << "!\n" ;
            return false ;
        }
    };
}

char AflLLVMPass::ID = 0;

static void
registerAflLLVMPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new AflLLVMPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerAflLLVMPass);
