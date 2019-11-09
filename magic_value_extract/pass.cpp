#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"

#include "llvm/IR/LLVMContext.h"

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/ADT/ilist.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>
#include <stdint.h>
using namespace llvm;


// Project Name : MagicValueExtract
//   (replace this with your NewProjectName )


namespace {
    struct MagicValueExtractPass : public BasicBlockPass {
        static char ID ;
        std::vector<std::pair<uint32_t, uint64_t>> magics;
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
            
            CmpInst* condition_pi ;
            if ( !( condition_pi = dyn_cast<CmpInst>(br_pi->getCondition())) ) {
                return false ;
            }
            
            errs() << "\n====================\n" ;
            condition_pi->print(errs()) ;
            for ( int i = 0 ; i < 2 ; ++i ) {
                Value* op = condition_pi->getOperand(i) ;
                if ( auto c = dyn_cast<Constant>(op) ) {
                    unsigned size = op->getType()->getPrimitiveSizeInBits() ;
                    errs() << "\nsize == "<< size << "\n\n" ;
                    uint64_t value = 0;
                    if ( auto cint = dyn_cast<ConstantInt>(c) ) {
                        value = cint->getValue().getLimitedValue();
                    //} else if ( auto cfp = dyn_cast<ConstantFP>(c) ) {
                    //    auto value = cfp->getValueAPF(); 
                    }
                    if ( value != 0 ) {
                        magics.push_back(std::make_pair(size, value));
                    }
                }
            }

            errs() << "\n====================\n" ;
            
            return false ;
        }
        virtual bool doFinalization(Module& M) {
            errs() << "\n";
            for ( auto it: magics ) {
                errs() << " # Size == " << it.first << ", Value == 0x";
                errs().write_hex(it.second) ;
                errs() << "\n" ;
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
