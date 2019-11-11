#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>
using namespace llvm;

// Project Name : PrintFunc
//   (replace this with your NewProjectName )

namespace {
    struct PrintFuncPass : public FunctionPass {
        static char ID ;
        GlobalVariable* glob_func_count ;
        PrintFuncPass() : FunctionPass(ID) {}
        
        virtual bool doInitialization(Module &M) {
            LLVMContext& context = M.getContext() ;
            glob_func_count = new GlobalVariable(M, IntegerType::getInt64Ty(context), false, GlobalValue::CommonLinkage,
                                                    0, "func_count") ;
            glob_func_count->setAlignment(4) ;
            ConstantInt* zero = ConstantInt::get(IntegerType::getInt64Ty(context), 1337) ;
            glob_func_count->setInitializer(zero) ;
        
        }

        virtual bool runOnFunction(Function &F) {
            LLVMContext& context = F.getContext() ;
            BasicBlock &EBB = F.getEntryBlock() ;
            BasicBlock::iterator IP = EBB.getFirstInsertionPt() ;
            IRBuilder<> IRB(&(*IP)) ;
            LoadInst* load_glob = IRB.CreateLoad(glob_func_count) ;
            Value *inc = IRB.CreateAdd(IRB.getInt64(1), load_glob);
            StoreInst *store = IRB.CreateStore(inc, glob_func_count);
            // printf
            Value* format_string = IRB.CreateGlobalStringPtr("N == %lu\n") ;
            LoadInst* another_load = IRB.CreateLoad(glob_func_count) ; // another load must be made. i.e. cannot use 'Value* inc' after store
            std::vector<Value*> arr ;
            arr.emplace_back(format_string) ;
            arr.emplace_back(another_load) ;
            Value* printf = F.getParent()->getOrInsertFunction("printf", Type::getVoidTy(context));
            CallInst* call_printf = IRB.CreateCall(printf, arr);

            
            // Debug
            errs() << F.getName() << "\n" ;
            
            for ( auto it = F.begin() ; it != F.end() ; ++it ) {
                errs() << *it << "\n" ; 
            }
            
            
            return true ;
        }
    };
}

char PrintFuncPass::ID = 0;

static void
registerPrintFuncPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new PrintFuncPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerPrintFuncPass);
