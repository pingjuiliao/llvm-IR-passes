#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <stdlib.h>

#define MAP_SIZE (1 << 16)

using namespace llvm;


// Project Name : AFL
//   (replace this with your NewProjectName )

namespace {

    struct AFLPass : public FunctionPass {
        static char ID ;
        AFLPass() : FunctionPass(ID) {}
        GlobalVariable* afl_area_ptr; // 64B coverage bitmap
        GlobalVariable* afl_prev_loc; // prev_loc
        virtual bool doInitialization(Module &M) {
            LLVMContext& C = M.getContext() ;
            afl_area_ptr = new GlobalVariable(M, PointerType::get(IntegerType::getInt8Ty(C), 0), false, GlobalValue::ExternalLinkage, 0, "__afl_area_ptr");

            afl_prev_loc = new GlobalVariable(M, IntegerType::getInt32Ty(C), false, GlobalValue::ExternalLinkage, 0, "__afl_prev_loc", 0, GlobalVariable::GeneralDynamicTLSModel, 0, false) ;
            errs() << "initialization Done" << "\n" ;
        }
        virtual bool runOnFunction(Function &F) {
            LLVMContext& C = F.getContext() ;
                 
            for ( auto &BB: F ) {
                BasicBlock::iterator IP = BB.getFirstInsertionPt() ; // BB == BB_check
                BasicBlock* BB_fail = BB->splitBasicBlock(IP, "bb_no_afl_area") ; 
                BasicBlock* BB_ret  = BB_check->splitBasicBlock(IP, "bb_")

                // IRBuilder<> IRB(&(*IP)) ;
                // %cur_loc = <compile_time_random>
                // afl_area_ptr[] 
                
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
