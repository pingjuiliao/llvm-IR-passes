#include "llvm/Pass.h"
#include "llvm/IR/Function.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/SmallVector.h"

#include "llvm/IR/InstIterator.h"
#include <vector>
#include <set>

using namespace llvm;

// Project Name : GepAnalysis
//   (replace this with your NewProjectName )

namespace {
    struct GepAnalysisPass : public FunctionPass {
        static char ID ;
        std::set<Value*> bt_visited ;
        // std::set<Value*> ft_visited ; 
        GepAnalysisPass() : FunctionPass(ID) {}
        virtual bool doInitialization(Module &M) {
            bt_visited = std::set<Value*>() ;
        }

        void backtrackToAlloca(User* inst, std::vector<Value*> &v) {
            AllocaInst* alloca = NULL;
            if ( bt_visited.find(inst) != bt_visited.end() ) {
                // errs() << *(bt_visited.find(inst)) << " bt_visited \n" ;
                return ;
            }
            // not found
            bt_visited.insert(inst) ;
            // isAllocaInst()
            if ( auto palloca = dyn_cast<AllocaInst>(inst) ) {
                if ( palloca->getAllocatedType()->isIntegerTy() )
                    v.push_back(inst) ;   
                return ;
            }
            
            // errs() << "\nPrinting out instructions " << *inst << " \n";
            for ( unsigned i = 0 ; i <  inst->getNumOperands() ; ++ i ) {;
                Value* op = inst->getOperand(i) ;
                // errs() << *op  ;
                if ( auto opinst = dyn_cast<Instruction>(op) ) {
                    // errs() << "   backtracking \n " << "============\n" ; 
                    backtrackToAlloca(opinst, v) ; 

                } else if ( auto pconst = dyn_cast<Constant>(op) ) {
                    // errs() << "is constant\n";
                    // ignore
                } else {
                    errs() << "[ERROR] backtrackToAlloca(): Not Captured Instruction\n" ;
                }
            }
        }
        
        virtual bool runOnFunction(Function &F) {
            std::vector<Value*> geps ;
            std::vector<Value*> allocas ; 
            
            // Basic Block Level Instrumentation
            for ( auto pbb = F.begin(); pbb != F.end() ; pbb ++ ) {
                for ( auto pi = pbb->begin() ; pi != pbb->end() ; pi ++ ) {
                    if ( auto gepi = dyn_cast<GetElementPtrInst>(&*pi)  ) {
                        geps.push_back(&(*gepi) );
                        backtrackToAlloca(gepi, allocas) ;
                    }
                }
            }

            if ( allocas.size() == 0 ) {
                return false ;
            }

            errs() << "############################\n" ;
            errs() << "## Function " << F.getName() << "\n" ;
            errs() << "############################\n" ;
            
            std::set<Value*> expanded = std::set<Value*>() ;    
            for ( auto alloca = allocas.begin() ; alloca != allocas.end() ; ++alloca ) {
                errs() << **alloca << "\n  \\ \n" ;
                expanded.clear() ;
                expanded.insert(*alloca) ;
                for ( inst_iterator it_ins = inst_begin(F), E = inst_end(F); it_ins != E ; ++it_ins ) {
                    bool related = false ;
                    for ( unsigned i = 0; i < it_ins->getNumOperands() ; ++i ) {
                        Value* op = it_ins->getOperand(i) ;
                        if ( expanded.find(op) != expanded.end() ) {
                            related = true ;
                        }
                    }
                    if ( related ) {
                        expanded.insert(&(*it_ins)) ;
                        errs() << "   |" << *it_ins << "\n" ;
                    }
                    
                }
                errs() << "   ----------------------------------------------------------\n" ;
            } 
            
            /*
            for ( auto it = geps.begin() ; it != geps.end() ; ++it ) {
                errs() << **it << "\n" ;
            }*/

            errs() << "\n\n" ;
            return false ;
        }
        virtual bool doFinalization(Module &M) {
        }
    };
}

char GepAnalysisPass::ID = 0;

static void
registerGepAnalysisPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new GepAnalysisPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerGepAnalysisPass);

