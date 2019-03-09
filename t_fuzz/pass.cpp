#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
using namespace llvm;

// Project Name : CmpReduce
//   (replace this with your NewProjectName )

namespace {
    struct CmpReducePass : public BasicBlockPass {
        static char ID ;
        CmpReducePass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock &B) {
            errs() << "\n\n#########################\n" ;
            errs() << "###  Basic Block \n";
            errs() << "#########################\n" ;
                
            // catch br 
            Instruction* termi_pi = B.getTerminator();
            if ( auto br_pi = dyn_cast<BranchInst>(termi_pi) ) {
                br_pi->swapSuccessors();  
            
            }



            // catch call 
            for (auto &I : B) {
                if ( isa<CallInst>(&I) ) {
                    I.print(errs()); 
                }
            }
            errs() << "\n\n" ;
            
            for (auto &I : B) {
                I.print(errs());
                errs() << "\n" ;
            }
            return true ;
        }
    };
}

char CmpReducePass::ID = 0;

static void
registerCmpReducePass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new CmpReducePass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerCmpReducePass);
