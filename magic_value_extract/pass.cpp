#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

// Project Name : MagicValueExtract
//   (replace this with your NewProjectName )

namespace {
    struct MagicValueExtractPass : public BasicBlockPass {
        static char ID ;
        MagicValueExtractPass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock &B) {
            errs() << "##############################\n" ;
            errs() << "## Here's another basic block \n" ;
            errs() << "##############################\n" ;
            for ( BasicBlock::iterator it = B.begin() ; it != B.end() ; ++ it) {
                errs() << *&*it << "\n" ;
            }
        }
    };
}

char MagicValueExtractPass::ID = 0;

static void
registerMagicValueExtractPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new MagicValueExtractPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerMagicValueExtractPass);
