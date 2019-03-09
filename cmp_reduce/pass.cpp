#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/LLVMContext.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/ADT/ilist.h"
using namespace llvm;

// Project Name : CmpReduce
//   (replace this with your NewProjectName )

namespace {
    struct CmpReducePass : public BasicBlockPass {
        static char ID ;
        CmpReducePass() : BasicBlockPass(ID) {}

        virtual bool runOnBasicBlock(BasicBlock &B) {
                
            LLVMContext& context = B.getContext();
            // catch br 
            Instruction* termi_pi = B.getTerminator();
            BranchInst* br_pi ;
            CmpInst* cmp_pi ;
            if ( !( br_pi = dyn_cast<BranchInst>(termi_pi) ) )  
                return false ;
            if ( br_pi->isUnconditional() ) 
                return false ;

            Value* cond = br_pi->getCondition() ;
            if ( !( cmp_pi = dyn_cast<CmpInst>(cond)) ) 
                return false ;
            
            if ( !cmp_pi->isEquality() )
                return false ;

            errs() << "\n\n#########################\n" ;
            errs() << "###  Basic Block \n";
            errs() << "#########################\n" ;
               
            // backtrace the cmp    
            CallInst* str_cmp_inst = NULL ;
            LoadInst* load_inst = NULL;
            ConstantInt* magic_value = NULL ;
            
            for ( int i = 0 ; i < 2 ; ++i ) {
                Value* v = cmp_pi->getOperand(i);
                     
                if ( auto callinst = dyn_cast<CallInst>(v) ) {
                    str_cmp_inst = callinst ; 
                } else if ( auto l = dyn_cast<LoadInst>(v) ) {
                    load_inst = l ;
                } else if ( auto c = dyn_cast<ConstantInt>(v) ) {
                    magic_value = c ;
                } else {
                    errs() << "This instruction is not handled : " ;
                    v->print(errs()) ;
                    errs() << "\n" ;
                    return false ;
                }
            }
            
            // if we got here, the codes must be changed
            //

            // Magic Value
            if ( load_inst && magic_value ) {
                // Magic Value case
                
                
                // to-do get original comparason data
                Value* user_input = load_inst->getPointerOperand();
                // to-do determine size
                unsigned size = magic_value->getType()->getBitWidth() / 8 ;
                if ( size < 4 ) {
                    return false ;
                }
                uint64_t magic_value_uint = magic_value->getValue().getLimitedValue();
                // to-do create blocks
                BasicBlock *bb = cmp_pi->getParent();
                for ( unsigned i = 0 ; i < size ; ++i ) {
                    BasicBlock *next_bb = bb->splitBasicBlock(cmp_pi);
                    IRBuilder<> builder(bb);
                    LoadInst* aligned_load = builder.CreateAlignedLoad(IntegerType::getInt8Ty(context), user_input, i+1) ;
                    
                    uint8_t compared_uint8 = (uint8_t) (( magic_value_uint >> (i * 8) ) & 0xff ); 
                    ConstantInt* compared_const = ConstantInt::get(IntegerType::getInt8Ty(context), compared_uint8) ;
                    Value* new_cmp_inst = builder.CreateICmpEQ(aligned_load, compared_const);
                    BranchInst* new_br_inst = builder.CreateCondBr(new_cmp_inst, br_pi->getSuccessor(0), br_pi->getSuccessor(1));
                    bb = next_bb ;
                }
                // to-do delete original
                // cmp_inst->eraseFromParent();
                // br_inst->eraseFromParent();
                
            } 
            
            
            // strcmp case !
            if ( str_cmp_inst && magic_value && magic_value->getValue() == 0) {
                errs() << "strcmp case\n" ;
            
            }
            
            
            // Debug : show IRs
            errs() << "\n\n" ;
            
            for (auto &I : B) {
                I.print(errs());
                errs() << "\n" ;
            }
            return true ;
        }
    };
}

char CmpReducePass::ID = 0;

static void
registerCmpReducePass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new CmpReducePass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerCmpReducePass);
