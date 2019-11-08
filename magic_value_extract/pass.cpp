#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/InstrTypes.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>
using namespace llvm;


// Project Name : MagicValueExtract
//   (replace this with your NewProjectName )

namespace {
    struct MagicValueExtractPass : public BasicBlockPass {
        static char ID ;
        MagicValueExtractPass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock &B) {
            
            LLVMContext& context = B.getContext() ;
            Instruction* termi_pi = B.getTerminator();
            BranchInst* br_pi ;
            if ( !(br_pi = dyn_cast<BranchInst>(termi_pi) ))
                return false ;
            if ( br_pi->isUnconditional() )
                return false ;
            
            errs() << "\n" ;
            errs() << "##############################\n" ;
            errs() << "## Here's another basic block \n" ;
            errs() << "##############################\n" ;
            
            Value* condition_pi ;
            if ( !( condition_pi = dyn_cast<CmpInst>(br_pi->getCondition())) ) {
                return false ;
            }
            
            errs() << "\n====================\n" ;
            condition_pi->print(errs()) ;
            
            // predicate
            // Predicate predicate = condition_pi->getPredicate() ;
            
            // operands
            Value* op0 = condition_pi->getOperand(0) ;
            Value* op1 = condition_pi->getOperand(1) ;
            op0->print(errs()) ;
            op1->print(errs()) ;

            errs() << "\n====================\n" ;

            for ( BasicBlock::reverse_iterator it = B.rbegin() ; it != B.rend() ; ++ it) {
                errs() << *it << "\n" ;

            }
            return false ;
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
