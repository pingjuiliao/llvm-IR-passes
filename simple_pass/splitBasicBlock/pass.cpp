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
            BasicBlock& B = F.getEntryBasicBlock() ;
            BasicBlock::iterator IP = BB.getFirstInsertionPt() ;
            BasicBlock& succ = B.splitBasicBlock(IP, "succ0") ;
            BasicBlock& succ1 = B.splitBasicBlock(IP, "succ1") ;
            // IRB
            IRBuilde

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
