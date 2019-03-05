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
            bool did_change = false ; 
            bool first = true ;
            for ( auto &B: F ) {
                for ( auto &I: B ) {
                    IRBuilder<> builder(&I) ;
                    Value* push_canary = builder.CreateAlloca();
                    /* if ( auto *op = dyn_cast<BinaryOperator>(&I) ) {

                        errs() << "Did make the change \n";
                        IRBuilder<> builder(op) ;
                        Value* x = op->getOperand(0);
                        Value* xor = builder.CreateXor(x, x);
                        op->setOperand(xor, 1);
                        did_change = true ; 
                        break ;
                    }*/
                }
                if ( did_change ) {
                    break ;
                }
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
