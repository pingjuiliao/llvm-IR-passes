#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
using namespace llvm;

// Class Name : OnlyOperatorMultiplyClass
namespace {
    struct OnlyOperatorMultiplyPass : public FunctionPass {
        static char ID ;
        OnlyOperatorMultiplyPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            for (auto &B: F) {
                for (auto &I : B) {
                    if (auto *op = dyn_cast<BinaryOperator>(&I)) {
                        IRBuilder<> builder(op);
                        Value *lhs = op->getOperand(0);
                        Value *rhs = op->getOperand(1);
                        Value *mul = builder.CreateMul(lhs, rhs);

                        for (auto &U : op->uses()) {
                            User *user = U.getUser();
                            user->setOperand(U.getOperandNo(), mul);
                        }
                    }
                }
            }
            return true ; // we modified the code
        }
    };
}

char OnlyOperatorMultiplyPass::ID = 0;

static void
registerOnlyOperatorMultiplyPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new OnlyOperatorMultiplyPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerOnlyOperatorMultiplyPass);
