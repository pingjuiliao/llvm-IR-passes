#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

// Project Name : AFL
//   (replace this with your NewProjectName )

namespace {
    struct AFLPass : public ModulePass {
        static char ID ;
        AFLPass() : ModulePass(ID) {}

        virtual bool runOnModule(Module &M) {
            LLVMContext& context = M.getContext() ;
            
            return true ;
        }
    };
}

char AFLPass::ID = 0;

static void
registerAFLPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new AFLPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_OptimizerLast, registerAFLPass);
//RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerAFLPass);
