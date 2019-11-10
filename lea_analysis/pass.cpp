#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>
using namespace llvm;

// Project Name : EffectiveAddressAnalysis
//   (replace this with your NewProjectName )

namespace {
    struct EffectiveAddressAnalysisPass : public FunctionPass {
        static char ID ;
        EffectiveAddressAnalysisPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
            // get all lea
            int i = 0;
            // SmallVector<Instruction&, 8> gep;
            std::vector<Instruction*> gep ; 
            // ArrayRef<Instruction> gepr;
            for ( auto pbb = F.begin(); pbb != F.end() ; pbb ++ ) {
                for ( auto pi = pbb->begin() ; pi != pbb->end() ; pi ++ ) {
                    if ( pi->getOpcode() != Instruction::GetElementPtr )    
                        continue ; 
                    
                    // GEP =  base, multiplier + offset ( op1*op2+op3 ) 
                    int tainted = 0 ;
                    for ( unsigned i = 1 ; i < pi->getNumOperands() ; i ++ ) {
                        auto operand = pi->getOperand(i) ;
                        // if constant
                        if ( auto con = dyn_cast<Constant>(operand) ) {
                            continue ;
                        }
                        tainted = 1 ;
                    }
                    if ( !tainted ) 
                        continue;
                    if ( auto pins = dyn_cast<Instruction>(pi) ) {
                        gep.push_back(pins);
                    }
                    // gepr.
                    // errs() << *pi  ;
                    // errs() << pi->getNumOperands() << "\n";
                            
                }
            }
            if ( gep.size() == 0 ) 
                return false ;

            errs() << "############################\n" ;
            errs() << "## Function " << F.getName() << "\n" ;
            errs() << "############################\n" ;
            
            for ( auto it = gep.begin() ; it != gep.end() ; ++it ) {
                errs() << **it << "\n" ;
                // errs() << (*it)->getNumOperands() << "\n" ;
            }

            // debug
            //errs() << "\n\n" ;
            //for ( auto pbb = F.begin() ; pbb!= F.end() ; pbb ++ ) {
            //    errs() << *pbb << "\n" ;
            //}
            errs() << "\n\n" ;
            return false ;
        }
    };
}

char EffectiveAddressAnalysisPass::ID = 0;

static void
registerEffectiveAddressAnalysisPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new EffectiveAddressAnalysisPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerEffectiveAddressAnalysisPass);
