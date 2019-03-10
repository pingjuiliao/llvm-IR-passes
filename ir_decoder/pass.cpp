#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/IR/InstIterator.h"
using namespace llvm;

// Project Name : PrintFunc
//   (replace this with your NewProjectName )

namespace {
    struct PrintFuncPass : public FunctionPass {
        static char ID ;
        PrintFuncPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            
            errs() << "\n\n##################################\n" ;
            errs() << "#####   " << F.getName() ;
            errs() << "\n####################################\n" ;
            // iterate through whole function            
            for( inst_iterator it  = inst_begin(F), E= inst_end(F); it != E; ++it ) {
                errs() << *it << "\n" ;
            }

            return false ;
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
