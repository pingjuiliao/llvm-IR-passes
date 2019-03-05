#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"
using namespace llvm;

namespace {
    struct StackGuardPass : public FunctionPass {
        static char ID ;
        StackGuardPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            errs() << "##############################\n" ;
            errs() << "#### " << F.getName() << "(..)\n" ;
            errs() << "##############################\n" ;

            for ( auto &B: F ) {
                for ( auto &I: B ) {
                    if ( auto *op = dyn_cast<BinaryOperator>(&I) ) {
                        IRBuilder<> builder(op) ;
                        Value* x = op->getOperand(0);
                        Value* or1 = builder.CreateOr(x, x);
                        // Value* or2 = builder.CreateOr(x, x);
                        // pi->getParent()->getInstList().insert(pi, or1);
                        if ( auto pi = dyn_cast<Instruction>(or1) )
                            pi->insertAfter(&I);
                        break ;
                    }
                }
                break ; 
            }
            
            for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
                  errs() << *I << "\n";


            return true ;
        }
    };
}

char StackGuardPass::ID = 0;

static void
registerStackGuardPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new StackGuardPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerStackGuardPass);
