#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

// Project Name : ExampleModule
//   (replace this with your NewProjectName )

namespace {
    struct ExampleModulePass : public ModulePass {
        static char ID ;
        ExampleModulePass() : ModulePass(ID) {}

        virtual bool runOnModule(Module &M) {
            errs() << "Started\n" ;
            LLVMContext& C = M.getContext() ;
            GlobalVariable* G = new GlobalVariable(M, IntegerType::getInt64Ty(C), false, GlobalValue::CommonLinkage, 0, "bb_count") ;
            ConstantInt *leet = ConstantInt::get(IntegerType::getInt64Ty(C), 1337) ;
            G->setInitializer(leet) ;
            for (auto &F: M) {
                errs() << F.getName() << "\n" ;
                
                for ( auto &BB: F ) {
                    BasicBlock::iterator IP = BB.getFirstInsertionPt() ;
                    IRBuilder<> IRB(&(*IP)) ;
                    LoadInst* load = IRB.CreateLoad(G) ;
                    Value* inc = IRB.CreateAdd(IRB.getInt64(1), load) ;
                    StoreInst* store = IRB.CreateStore(inc, G) ;
                }
            }

            return true ;
        }
    };
}

char ExampleModulePass::ID = 0;

static void
registerExampleModulePass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new ExampleModulePass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_OptimizerLast, registerExampleModulePass) ;
//RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerExampleModulePass);

