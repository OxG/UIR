#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Assembly/AssemblyAnnotationWriter.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Type.h"
#include "string.h"
#include "llvm/IR/Constants.h"
//#include "llvm/Support/DebugLoc.h"
using namespace llvm;

namespace {
struct Uir : public ModulePass {
    static char ID;
    Uir() : ModulePass(ID) {}
    int count=0;
    BlockAddress * MasBA[214748364];
    virtual bool runOnModule(llvm::Module &M) {

        char * TID[20] = {
            "VoidTyID" , "HalfTyID", "FloatTyID", "DoubleTyID",
            "X86_FP80TyID", "FP128TyID", "PPC_FP128TyID", "LabelTyID",
            "MetadataTyID", "X86_MMXTyID", "IntegerTyID", "FunctionTyID",
            "StructTyID", "ArrayTyID", "PointerTyID", "VectorTyID",
            "NumTypeIDs", "LastPrimitiveTyID = X86_MMXTyID", "FirstDerivedTyID = IntegerTyID"
        };
        for (llvm::Module::iterator mn = M.begin(), me = M.end();
             mn != me;
             ++mn)
        {

            errs() << "Hello: ";
            errs().write_escaped(mn->getName().str())<<"(";
            if(mn->arg_empty()==false)
            {
                for(llvm::Function::arg_iterator aie=mn->arg_begin(), ain=mn->arg_end();aie!=ain;++aie)
                {
                    errs().write_escaped(" ")<<TID[aie->getType()->getTypeID()];
                }

            }
            else
            {
                errs().write_escaped("*arg_empty!*");
            }
            errs().write_escaped(" ")<<")";

            errs().write_escaped("::CallingConv::")<<mn->getCallingConv()<<"\n";
            if(mn->empty()==false)
            {
                llvm::BasicBlock * BB = &mn->getEntryBlock();
                errs().write_escaped("Block ")<<BlockAddress::get(BB)<<"\n";
                MasBA[0]=BlockAddress::get(BB);
                count++;
                Tree(BB);
            }




        }



        return false;
    }
    bool Tree(llvm::BasicBlock * BB)
    {
        llvm::TerminatorInst * TI;
        llvm::BasicBlock * NBB;
        if(BB->empty()==false)
        {
            for(llvm::BasicBlock::iterator inn=BB->begin(), ine=BB->end(); inn!=ine ;++inn)
            {
                errs().write_escaped(inn->getOpcodeName () )<<" ";
                for (User::op_iterator i = inn->op_begin(), e = inn->op_end(); i != e; ++i)
                {
                    Value * v = *i;
                    errs().write_escaped( v->getName())<<" ";
                }
                errs().write_escaped(" ")<<"\n";

            }


            TI=BB->getTerminator();
            unsigned int NS= TI->getNumSuccessors();

            for(int i=0;i<NS;i++)
            {
                bool tam=false;
                NBB=TI->getSuccessor(i);
                for(int o=0;o<=count;o++)
                {
                    if(MasBA[o]==BlockAddress::get(NBB))
                    {
                        tam=true;
                    }
                }
                if(tam==true)
                {
                }
                else
                {

                    errs().write_escaped("Block ")<<BlockAddress::get(NBB)<<"\n";
                    count++;
                    MasBA[count]=BlockAddress::get(NBB);
                    Tree(NBB);
                }
            }
        }

    };
    
    
};
}

char Uir::ID = 0;
static RegisterPass<Uir> X("uir", "uir pass", false, false);
