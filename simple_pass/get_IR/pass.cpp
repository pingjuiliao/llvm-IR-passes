#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

namespace {
    struct GetIRPass : public FunctionPass {
        static char ID ;
        GetIRPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            errs() << "In a function called " << F.getName() << "!\n" ;
            errs() << "Function Body :\n" ;
            F.print(llvm::errs());
            
            for (auto &B: F) {
                errs() << "Basic Block:\n" ;
                B.print(llvm::errs(), true);
                for (auto &I: B) {
                    errs() << "Instruction: " ;
                    I.print(llvm::errs(), true);
                    errs() << "\n" ;
                }
            }


            return false ;
        }
    };
}

char GetIRPass::ID = 0;

static void
registerGetIRPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new GetIRPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerGetIRPass);
