#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
using namespace llvm;

// Project Name : TFuzz
//   (replace this with your NewProjectName )

namespace {
    struct TFuzzPass : public BasicBlockPass {
        static char ID ;
        TFuzzPass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock &B) {
            errs() << "\n\n#########################\n" ;
            errs() << "###  Basic Block \n";
            errs() << "#########################\n" ;
                
            // catch br 
            Instruction* termi_pi = B.getTerminator();
            
            if ( BranchInst *br_pi = dyn_cast<BranchInst>(termi_pi) ) {
                if ( br_pi->isUnconditional() ) {
                    return false ;
                }
                // catch condition !
                Value* v = br_pi->getCondition();
                if ( CmpInst *cmp_pi = dyn_cast<CmpInst>(v) ) {
                    if ( cmp_pi->isEquality() ) {
                        br_pi->swapSuccessors(); // amazing magic  
                    }
                }
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

char TFuzzPass::ID = 0;

static void
registerTFuzzPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new TFuzzPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerTFuzzPass);
