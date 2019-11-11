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
        uint64_t canary = 0x00c01db7e3c0ffee ;
        GlobalVariable* glob_canary;
        
        StackGuardPass() : FunctionPass(ID) {}

        virtual bool doInitialization(Module &M) {
            LLVMContext& context = M.getContext() ;
            ConstantInt* canary_const = ConstantInt::get(IntegerType::getInt64Ty(context), canary);
            ConstantInt* const_int_val = ConstantInt::get(context, APInt(32,0x41));
            
            glob_canary = new GlobalVariable(M, IntegerType::getInt64Ty(context), /*isConst*/ false,
                    GlobalValue::CommonLinkage, 0, "canary");
            glob_canary->setAlignment(8) ;
            glob_canary->setInitializer(const_int_val);
            glob_canary->setExternallyInitialized(true);
            errs() << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" ;
            errs() << "@@@@  global value : \n" ;
            errs() << "@@@@@@@@@@@@@@@@@@@@@@@@\n";
            glob_canary->print(errs());
            errs() << "\n";
            // glob_canary->setVisibility(GlobalValue::VisibilityTypes::ProtectedVisibility); 
        }
        virtual bool runOnFunction(Function &F) {
            errs() << "##############################\n" ;
            errs() << "#### " << F.getName() << "(..)\n" ;
            errs() << "##############################\n" ;
            glob_canary->print(errs()) ;
            errs() << "\n\n--------------------------\n" ;
            LLVMContext& context = F.getContext() ;
            
            // push the canary
            BasicBlock &B = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI();
            IRBuilder<> proloque_builder(pi) ;
            // comparison vs. constant
            LoadInst* load_glob = proloque_builder.CreateLoad(glob_canary) ; //new
            ConstantInt* canary_const = ConstantInt::get(IntegerType::getInt64Ty(context), canary);
            AllocaInst* alloca_canary_inst = proloque_builder.CreateAlloca(IntegerType::getInt64Ty(context));
            //StoreInst* push_canary_inst   = proloque_builder.CreateStore(canary_const, alloca_canary_inst);
            StoreInst* push_canary_inst   = proloque_builder.CreateStore(load_glob, alloca_canary_inst); // new
            

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
            LoadInst* another_glob = check_bb_builder.CreateLoad(glob_canary);
            // Value* canary_cmp = check_bb_builder.CreateICmpEQ(load_canary_inst, canary_const);
            Value* canary_cmp = check_bb_builder.CreateICmpEQ(load_canary_inst, another_glob);
            
            check_bb_builder.CreateCondBr(canary_cmp, ret_bb, check_bb);

            
            IRBuilder<> stack_chk_fail_bb_builder(check_bb) ;
            // call puts(&buf);
            Value* puts = F.getParent()->getOrInsertFunction("puts", Type::getVoidTy(context));
            ConstantInt* zero = ConstantInt::get(IntegerType::getInt64Ty(context), 0) ;
            Value* address_of_canary  = stack_chk_fail_bb_builder.CreateGEP(glob_canary, zero);
            // LoadInst* load_address_of_canary = stack_chk_fail_bb_builder.CreateAlignedLoad(address_of_canary, 8) ;
            ArrayRef<Value *> puts_argv(address_of_canary);
            CallInst* call_puts = stack_chk_fail_bb_builder.CreateCall(puts, puts_argv); 
            

            // call exit(1);
            Value* exit = F.getParent()->getOrInsertFunction("exit", Type::getVoidTy(context));
            ConstantInt* one = ConstantInt::get(IntegerType::getInt64Ty(context), 1);
            ArrayRef<Value *> argsV(one);
            CallInst* call_exit = stack_chk_fail_bb_builder.CreateCall(exit, argsV); 
            // unreachable ;
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
