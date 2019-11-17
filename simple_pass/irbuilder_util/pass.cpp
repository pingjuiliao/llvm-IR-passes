#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstIterator.h" 
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include <vector>
using namespace llvm;

// Project Name : Util
//   (replace this with your NewProjectName )

namespace {
    struct UtilPass : public FunctionPass {
        static char ID ;
        UtilPass() : FunctionPass(ID) {}
        void instrumentAfter(Instruction* I) {
            LLVMContext& C = I->getContext() ;
            Module *M = I->getModule() ;
            Function* F = I->getFunction() ;
            Value* puts = M->getOrInsertFunction("puts", Type::getVoidTy(C)) ;


            
            IRBuilder<> IRB(I->getNextNode()) ;
            Value* str = IRB.CreateGlobalStringPtr("returned") ;
            ArrayRef<Value*> puts_argv(str);
            CallInst* call_puts = IRB.CreateCall(puts, puts_argv) ;
             
        }


        virtual bool runOnFunction(Function &F) {
            std::vector<Instruction*> orig_calls ;
            for ( inst_iterator it = inst_begin(F), E=inst_end(F); it != E ; ++it ) {
                if ( auto callinst = dyn_cast<CallInst>(&(*it)) ) {
                    // instrumentAfter(callinst);       
                    orig_calls.push_back(callinst) ;
                }
            }
            for ( auto it = orig_calls.begin() ; it != orig_calls.end() ; ++ it ) {
                instrumentAfter(*it) ;
            }
            for ( inst_iterator it = inst_begin(F), E=inst_end(F); it != E ; ++it ) {
                errs() << *it << "\n" ;
            }
            return true ;
        }
    };
}

char UtilPass::ID = 0;

static void
registerUtilPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new UtilPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerUtilPass);
