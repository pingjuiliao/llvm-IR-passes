#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
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
            // Now that
            //                              load xxxxxx
            //                    p_inst -> ret i32....
            //

            // to-do : create end_block 
            BasicBlock* bb = p_inst_right_before_ret->getParent();
            // to-do : create fail_block
            BasicBlock* check_bb= bb->splitBasicBlock(p_inst_right_before_ret, "check_bb");
            BasicBlock* stack_chk_fail_bb= check_bb->splitBasicBlock(p_inst_right_before_ret, "stack_chk_fail_bb");
            BasicBlock* ret_bb= stack_chk_fail_bb->splitBasicBlock(p_inst_right_before_ret, "ret_bb");
            
            bb->getTerminator()->eraseFromParent();
             
            
            
            IRBuilder<> check_bb_builder(bb);
            LoadInst* load_canary_inst = check_bb_builder.CreateLoad(IntegerType::getInt64Ty(context), push_canary_inst->getPointerOperand());
            check_bb->getTerminator()->eraseFromParent();
            Value* canary_cmp = check_bb_builder.CreateICmpEQ(load_canary_inst, canary_const);
            check_bb_builder.CreateCondBr(canary_cmp, ret_bb, check_bb);

            
            IRBuilder<> stack_chk_fail_bb_builder(check_bb) ;
            // stack_chk_fail_bb_builder.CreateLoad(alloca_canary_inst);
            Value* exit = F.getParent()->getOrInsertFunction("exit", Type::getVoidTy(context));
            ConstantInt* one = ConstantInt::get(IntegerType::getInt64Ty(context), 1);
            ArrayRef<Value *> argsV(one);
            
            CallInst* call_exit = stack_chk_fail_bb_builder.CreateCall(exit, argsV); 
            stack_chk_fail_bb_builder.CreateUnreachable();
            

            // For DEBUG : print IRs 
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
