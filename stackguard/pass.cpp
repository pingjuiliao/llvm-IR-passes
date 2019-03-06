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
#include "llvm/ADT/ilist.h"
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
            IRBuilder<> proloque_builder(pi) ;
            ConstantInt* canary_const = ConstantInt::get(IntegerType::getInt64Ty(context), canary);
            AllocaInst* alloca_canary_inst = proloque_builder.CreateAlloca(IntegerType::getInt64Ty(context));
            StoreInst* push_canary_inst   = proloque_builder.CreateStore(canary_const, alloca_canary_inst);

            // check : unfinished ...
            inst_iterator function_back = (--inst_end(F));
            Instruction *p_inst_right_before_ret = &*function_back;
            
            while ( !isa<ReturnInst>(p_inst_right_before_ret) ) {
                --function_back ; 
                p_inst_right_before_ret = &*function_back ;
                if ( function_back == inst_begin(F) ) {
                    break ;
                }
            }
            // p_inst_right_before_ret = &*(--function_back) ;
            // errs() << p_inst_right_before_ret->getOpcodeName() << "\n"; 
            
            // BasicBlock *pb = dyn_cast<BasicBlock>(p_inst_right_before_ret);
            
            IRBuilder<> epiloque_builder(p_inst_right_before_ret) ;
            // Value* l = dyn_cast<Value>(canary_cast) ;  // not gonna solve the problem
            LoadInst* load_canary_inst = epiloque_builder.CreateLoad(IntegerType::getInt64Ty(context), alloca_canary_inst);
            epiloque_builder.CreateICmpEQ(load_canary_inst, canary_const);
                       
            

            // For DEBUG : print IRs
            Value* canary_itself = push_canary->getPointerOperand();
            for (inst_iterator it = inst_begin(F), E= inst_end(F); it != E; ++it)
                  errs() << *it << "\n";
            


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
