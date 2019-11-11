#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
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
            for (auto &F: M) {
                errs() << F.getName() << "\n" ;
            } 
            // errs() << "I saw a function called " << F.getName() << "!\n" ;
            return false ;
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

