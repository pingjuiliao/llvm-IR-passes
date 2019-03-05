#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"

#include "llvm/IR/LLVMContext.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/IR/InstIterator.h"

#include "llvm/IR/IRBuilder.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

using namespace llvm;





namespace {
    struct StackGuardPass : public FunctionPass {
        static char ID ;
        uint64_t canary = 0xc01db7e3c0ffee00 ;
        
        
        StackGuardPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            errs() << "##############################\n" ;
            errs() << "#### " << F.getName() << "(..)\n" ;
            errs() << "##############################\n" ;
            LLVMContext& context = F.getContext() ;
            bool did_change = false ; 
            bool first = true ;
            
            // push the canary
            BasicBlock &B = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI();
            IRBuilder<> builder(pi) ;
            ConstantInt* canary_cast = ConstantInt::get(IntegerType::getInt64Ty(context), canary);
            AllocaInst* alloca_canary = builder.CreateAlloca(IntegerType::getInt64Ty(context));
            StoreInst* push_canary   = builder.CreateStore(canary_cast, alloca_canary);
            
            // check : unfinished ...
            

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
