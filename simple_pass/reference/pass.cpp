#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

// Project Name : PrintFunc
//   (replace this with your NewProjectName )

namespace {
    struct PrintFuncPass : public FunctionPass {
        static char ID ;
        PrintFuncPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            LLVMContext& C = F.getContext() ;
            Value* printf = F.getParent()->getOrInsertFunction("printf", Type::getVoidTy(C)) ;
            for(auto it = inst_begin(F) ; it != inst_end(F); ++it ) {
                if ( auto store = dyn_cast<StoreInst>(&(*it)) ) {
                    IRBuilder<> IRB(store->getNextNode()) ;
                    std::vector<Value*> v_printf ;
                    Value* fs = IRB.CreateGlobalStringPtr("Just assigned string %s\n");
                    v_printf.emplace_back(fs);
                    v_printf.emplace_back(store->getPointerOperand());
                   // v_printf.emplace_back(PointerType::get()) ; 
                    CallInst* call_printf = IRB.CreateCall(printf, v_printf) ; 
                }
            }
            return true; ;
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
