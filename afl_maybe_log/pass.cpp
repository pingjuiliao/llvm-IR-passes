#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <stdlib.h>
#include <vector>
#define MAP_SIZE (1 << 16)

using namespace llvm;


// Project Name : AFL
//   (replace this with your NewProjectName )

namespace {

    struct AFLPass : public FunctionPass {
        static char ID ;
        AFLPass() : FunctionPass(ID) {}
        GlobalVariable* afl_global_area_ptr ;
        GlobalVariable* afl_area_ptr; // 64B coverage bitmap
        GlobalVariable* afl_prev_loc; // prev_loc
        GlobalVariable* afl_setup_failure ; 
        GlobalVariable* afl_temp ; // prev_loc
        
        
        virtual bool doInitialization(Module &M) {
            LLVMContext& C = M.getContext() ;
            ConstantInt* const_zero = ConstantInt::get(IntegerType::getInt64Ty(C), 0) ; 
            afl_global_area_ptr = new GlobalVariable(M, PointerType::get(PointerType::get(IntegerType::getInt8Ty(C), 0), 0),false, GlobalValue::ExternalLinkage, const_zero, "__afl_global_area_ptr" ); 
            afl_area_ptr = new GlobalVariable(M, PointerType::get(IntegerType::getInt8Ty(C), 0), false, GlobalValue::ExternalLinkage, const_zero, "__afl_area_ptr");
            afl_prev_loc = new GlobalVariable(M, IntegerType::getInt32Ty(C), false, GlobalValue::ExternalLinkage, const_zero, "__afl_prev_loc", 0, GlobalVariable::GeneralDynamicTLSModel, 0, false) ;

            afl_temp = new GlobalVariable(M, IntegerType::getInt64Ty(C), false, GlobalValue::CommonLinkage, const_zero, "__afl_temp");
            afl_setup_failure = new GlobalVariable(M, IntegerType::getInt64Ty(C), false, GlobalValue::CommonLinkage, const_zero, "__afl_setup_failure") ;
            errs() << "initialization Done" << "\n" ;
        }

        virtual bool runOnFunction(Function &F) {
            
            LLVMContext& C = F.getContext() ;
            ConstantInt* const_zero = ConstantInt::get(IntegerType::getInt64Ty(C), 0) ; 
            ConstantInt* const_one  = ConstantInt::get(IntegerType::getInt64Ty(C), 1) ; 
            // LIBC functions
            Value* puts   = F.getParent()->getOrInsertFunction("puts", Type::getVoidTy(C)) ;
            Value* getenv = F.getParent()->getOrInsertFunction("getenv", PointerType::getInt8PtrTy(C)) ;
            Value* atoi   = F.getParent()->getOrInsertFunction("atoi", IntegerType::getInt32Ty(C)); 
            Value* shmat  = F.getParent()->getOrInsertFunction("shmat", PointerType::getInt8PtrTy(C)) ;
        
            
            errs() << "dealing with function " << F.getName() << "\n\n" ; 
            std::vector<BasicBlock*> original_blocks ;
            
            for ( auto &BB: F) {
                original_blocks.emplace_back(&BB) ;
            }
            for (std::vector<BasicBlock*>::iterator it = original_blocks.begin(); it != original_blocks.end(); ++it) {
                BasicBlock* afl_maybe_log = *it ;
                BasicBlock::iterator IP = afl_maybe_log->getFirstInsertionPt() ; // pBB == BB_check == "__afl_maybe_log"
                
                // split blocks 
                BasicBlock* afl_setup_check_failure = afl_maybe_log->splitBasicBlock(IP) ;
                BasicBlock* afl_setup_check_global  = afl_setup_check_failure->splitBasicBlock(IP) ;
                BasicBlock* afl_setup               = afl_setup_check_global->splitBasicBlock(IP) ;
                BasicBlock* afl_shm_getenv          = afl_setup->splitBasicBlock(IP);
                BasicBlock* afl_shm                 = afl_shm_getenv->splitBasicBlock(IP);
                BasicBlock* afl_shm_success         = afl_shm->splitBasicBlock(IP);
                BasicBlock* afl_store  = afl_shm_success->splitBasicBlock(IP);
                BasicBlock* afl_fail = afl_store->splitBasicBlock(IP) ;
                BasicBlock* afl_ret   = afl_fail->splitBasicBlock(IP);
                /******************
                 * afl_maybe_log
                 *****************/
                IRBuilder<> IRB_maybe_log(afl_maybe_log->getTerminator()) ;
                Value* fs_maybe = IRB_maybe_log.CreateGlobalStringPtr("__afl_maybe_log");
                ArrayRef<Value*> arr_maybe(fs_maybe) ;
                CallInst* call_maybe = IRB_maybe_log.CreateCall(puts, arr_maybe) ;
                
                LoadInst* load_afl_area_ptr = IRB_maybe_log.CreateLoad(afl_area_ptr) ; 
                Value* test_afl_area_ptr    = IRB_maybe_log.CreateICmpEQ(load_afl_area_ptr, ConstantPointerNull::get(PointerType::get(IntegerType::getInt8Ty(C), 0)));
                BranchInst* br_maybe_log    = IRB_maybe_log.CreateCondBr(test_afl_area_ptr, afl_setup_check_failure, afl_fail);
                afl_maybe_log->getTerminator()->eraseFromParent() ;

                /*******************
                 * afl_setup_check_failure
                 ******************/
                IRBuilder<> IRB_setup_check_failure(afl_setup_check_failure->getTerminator());
                Value* fs_setup_check_failure = IRB_setup_check_failure.CreateGlobalStringPtr("__afl_setup_chk_failure") ;
                ArrayRef<Value*> arr_setup_check_failure(fs_setup_check_failure) ;
                CallInst* call_setup_chk_failure = IRB_setup_check_failure.CreateCall(puts, arr_setup_check_failure) ;
                
                LoadInst* load_afl_setup_failure = IRB_setup_check_failure.CreateLoad(afl_setup_failure) ;
                Value* cmp_setup_failure = IRB_setup_check_failure.CreateICmpEQ(load_afl_setup_failure, const_zero) ;
                BranchInst* br_setup_failure = IRB_setup_check_failure.CreateCondBr(cmp_setup_failure, afl_setup_check_global, afl_ret);
                afl_setup_check_failure->getTerminator()->eraseFromParent() ;  
                
                
                /******************
                 * afl_setup_check_global
                 ******************/
                IRBuilder<> IRB_setup_check_global(afl_setup_check_global->getTerminator());
                Value* fs_setup_glob = IRB_setup_check_global.CreateGlobalStringPtr("__afl_setup_chk_global") ;
                ArrayRef<Value*> arr_setup_glob(fs_setup_glob) ;
                CallInst* call_setup_glob = IRB_setup_check_global.CreateCall(puts, arr_setup_glob) ;
                
                LoadInst* load_afl_setup_global = IRB_setup_check_global.CreateLoad(afl_global_area_ptr) ;
                Value* cmp_setup_global = IRB_setup_check_global.CreateICmpNE(load_afl_setup_global, ConstantPointerNull::get(PointerType::get(PointerType::get(IntegerType::getInt8Ty(C), 0), 0))) ;
                BranchInst* br_setup_global = IRB_setup_check_global.CreateCondBr(cmp_setup_global, afl_setup, afl_shm_getenv);
                afl_setup_check_global->getTerminator()->eraseFromParent() ;
                
                
                /**************************************
                 * afl_setup ( simply load the global )
                 **********************/
                IRBuilder<> IRB_setup(afl_setup->getTerminator()) ;
                Value* fs_setup = IRB_setup.CreateGlobalStringPtr("__afl_setup") ;
                ArrayRef<Value *> arr_setup(fs_setup) ;
                CallInst* call_setup = IRB_setup.CreateCall(puts, arr_setup) ;

                LoadInst* load_non_null_global_area_ptr = IRB_setup.CreateLoad(afl_global_area_ptr);
                LoadInst* load_dref_global     = IRB_setup.CreateLoad(load_non_null_global_area_ptr) ;
                StoreInst* store_to_afl_area_ptr = IRB_setup.CreateStore(load_dref_global, afl_area_ptr) ;
                
                /************************************
                 * afl_shm_getenv
                 * ****************/
                IRBuilder<> IRB_shm_getenv(afl_shm_getenv->getTerminator()) ;
                Value* fs_shm_getenv = IRB_shm_getenv.CreateGlobalStringPtr("__afl_shm_getenv") ;
                ArrayRef<Value*> arr_shm_getenv(fs_shm_getenv) ;
                CallInst* call_shm_getenv = IRB_shm_getenv.CreateCall(puts, arr_shm_getenv) ;

                Value* env_string = IRB_shm_getenv.CreateGlobalStringPtr("AFL_SHM_ENV");
                ArrayRef<Value*> getenv_argv(env_string) ;
                CallInst* call_getenv = IRB_shm_getenv.CreateCall(getenv, getenv_argv) ;
                Value* test_env = IRB_shm_getenv.CreateICmpNE(call_getenv, ConstantPointerNull::get(PointerType::getInt8PtrTy(C)));
                BranchInst* br_shm_env = IRB_shm_getenv.CreateCondBr(test_env, afl_shm, afl_fail) ;
                afl_shm_getenv->getTerminator()->eraseFromParent() ;

                /************************************
                 * afl_shm
                 * *****************/
                IRBuilder<> IRB_shm(afl_shm->getTerminator()) ;
                ArrayRef<Value*> arr_env_ref(call_getenv); // this debug info : puts(getenv("AFL_SHM_ENV")) ;
                CallInst* call_puts_env = IRB_shm.CreateCall(puts, arr_env_ref) ; 
                
                CallInst* call_atoi = IRB_shm.CreateCall(atoi, arr_env_ref);
                std::vector<Value*> v ; 
                v.emplace_back(call_atoi); 
                v.emplace_back(ConstantPointerNull::get(PointerType::getInt8PtrTy(C))) ; 
                v.emplace_back(ConstantInt::get(IntegerType::getInt8Ty(C), 0));
                CallInst* call_shmat = IRB_shm.CreateCall(shmat, v) ;
                Value* alloca_cast_space = IRB_shm.CreateAlloca(IntegerType::getInt64Ty(C)); 
                StoreInst*  store_inst   = IRB_shm.CreateStore(call_shmat, alloca_cast_space) ;
                LoadInst* load_shmat_cast= IRB_shm.CreateLoad(alloca_cast_space) ;
                
                Value* test_shm  = IRB_shm.CreateICmpNE(load_shmat_cast, ConstantInt::get(IntegerType::getInt64Ty(C), -1)); // shmat returns -1 if error
                BranchInst* br_shm = IRB_shm.CreateCondBr(test_shm, afl_shm_success, afl_fail);
                afl_shm->getTerminator()->eraseFromParent();
                
                /***********************************
                 * afl_shm_success
                 * ***************************/
                IRBuilder<> IRB_shm_success(afl_shm->getTerminator());
                Value* fs_shm_success = IRB_shm_success.CreateGlobalStringPtr("__afl_shm_success");
                ArrayRef<Value*> arr_shm_success(fs_shm_success);
                CallInst* call_shm_success = IRB_shm_success.CreateCall(puts, fs_shm_success) ;
                
                StoreInst* store_afl_area_ptr = IRB_shm_success.CreateStore(call_shmat, afl_area_ptr);
                LoadInst* load_afl_global_area_ptr = IRB_shm_success.CreateLoad(afl_global_area_ptr) ;
                LoadInst* load_dref_global_area_ptr= IRB_shm_success.CreateLoad(load_afl_global_area_ptr);
                StoreInst* store_afl_global_area_ptr = IRB_shm_success.CreateStore(call_shmat, afl_global_area_ptr);
                // TODO: Fork Server

                /**************************************
                 * afl_store
                 * ****************************/
                IRBuilder<> IRB_store(afl_store->getTerminator());
                
                // make up cur_loc
                unsigned int cur_loc = random() % MAP_SIZE ;
                ConstantInt *CurLoc = ConstantInt::get(IntegerType::getInt32Ty(C), cur_loc) ;
                
                // load prev_loc
                LoadInst* PrevLoc = IRB_store.CreateLoad(afl_prev_loc);
                PrevLoc->setMetadata(F.getParent()->getMDKindID("nosanitize"), MDNode::get(C, None));
                Value* PrevLocCasted = IRB_store.CreateZExt(PrevLoc, IRB_store.getInt32Ty());

                // load SHM pointer
                LoadInst* MapPtr = IRB_store.CreateLoad(afl_area_ptr);
                MapPtr->setMetadata(F.getParent()->getMDKindID("nosanitize"), MDNode::get(C, None));
                Value* MapPtrIdx = IRB_store.CreateGEP(MapPtr, IRB_store.CreateXor(PrevLocCasted, CurLoc));

                // Update bitmap
                LoadInst* Counter = IRB_store.CreateLoad(MapPtrIdx);
                Counter->setMetadata(F.getParent()->getMDKindID("nosanitize"), MDNode::get(C, None));
                Value *Incr = IRB_store.CreateAdd(Counter, ConstantInt::get(IntegerType::getInt8Ty(C), 1));

                StoreInst* write_map = IRB_store.CreateStore(Incr, MapPtrIdx) ;
                write_map->setMetadata(F.getParent()->getMDKindID("nosanitize"), MDNode::get(C, None));

                // Set prev_loc to cur_loc >> 1
                StoreInst* Store = 
                    IRB_store.CreateStore(ConstantInt::get(IntegerType::getInt32Ty(C), cur_loc >> 1), afl_prev_loc);
                Store->setMetadata(F.getParent()->getMDKindID("nosanitize"), MDNode::get(C, None));

                // inst_block ++ // number of instrumented blocks


                /***********************************
                 * afl_fail
                 * *******************************/
                IRBuilder<> IRB_fail(afl_fail->getTerminator());
                Value* fs_fail = IRB_fail.CreateGlobalStringPtr("__afl_fail") ;
                ArrayRef<Value*> arr_fail(fs_fail) ;
                CallInst* call_fail = IRB_fail.CreateCall(puts, arr_fail) ;
                StoreInst* store_failure = IRB_fail.CreateStore(const_one, afl_setup_failure) ; 

                /************************************
                 * afl_ret
                 * ***********************************/
                // The original basic block
            }

            return true ;
        }
    };
}

char AFLPass::ID = 0;

static void
registerAFLPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new AFLPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerAFLPass);
// RegisterMyPass(PassManagerBuilder::EP_OptimizerLast, registerAFLPass);
