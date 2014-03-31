#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <set>
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
#include <string>
#include "llvm/IR/Constants.h"
//#include "llvm/Support/DebugLoc.h"
using namespace std;
using namespace llvm;

namespace {
struct Uir : public ModulePass {
    static char ID;
    Uir() : ModulePass(ID) {}
   std::set< BlockAddress* > MasBA;
   char * TID[20] = {
       "VoidTyID" , "HalfTyID", "FloatTyID", "DoubleTyID",
       "X86_FP80TyID", "FP128TyID", "PPC_FP128TyID", "LabelTyID",
       "MetadataTyID", "X86_MMXTyID", "IntegerTyID", "FunctionTyID",
       "StructTyID", "ArrayTyID", "PointerTyID", "VectorTyID",
       "NumTypeIDs", "LastPrimitiveTyID = X86_MMXTyID", "FirstDerivedTyID = IntegerTyID"
   };

    llvm::Function& findfunc(llvm::Module &M,string find)
{
        errs()<<"\n poisk "<<find<<"";
        for (llvm::Module::iterator mn = M.begin(), me = M.end();
             mn != me;
             ++mn)
        {

            if(mn->getName().str().compare(find)==0)
            {
                return *mn;
            }
        }
        errs()<<"\nOut of module!";
        llvm::Function& ret=*M.begin();
        return ret;

    }
    void dumpfheader(llvm::Function &mn)
    {
        errs() << "\n Function: ";
        errs().write_escaped(mn.getName().str())<<"(";
        if(mn.arg_empty()==false)
        {
            for(llvm::Function::arg_iterator aie=mn.arg_begin(), ain=mn.arg_end();aie!=ain;++aie)
            {
                errs().write_escaped(" ")<<TID[aie->getType()->getTypeID()];
            }

        }
        else
        {
            errs().write_escaped("*arg_empty!*");
        }
        errs().write_escaped(" ")<<")";

        errs().write_escaped("::CallingConv::")<<mn.getCallingConv()<<"\n";

    }

    virtual bool runOnModule(llvm::Module &M) {


        llvm::Function& mn=findfunc(M,"main");

        dumpfheader(mn);
            if(!mn.empty())
            {
                llvm::BasicBlock * BB = &mn.getEntryBlock();
                Tree(BB,M);
            }
        return false;
    }
//    llvm::BB get() {}
    //void func() { BB* = &get(); }
    bool Tree(llvm::BasicBlock * BB,llvm::Module &M)
    {
        if(MasBA.find(BlockAddress::get(BB)) != MasBA.end())
        {
            return false;
        }
        MasBA.insert(BlockAddress::get(BB));
        errs().write_escaped("Block ")<<BlockAddress::get(BB)<<"\n";

        llvm::TerminatorInst * TI;
        llvm::BasicBlock * NBB;
        if(BB->empty()==false)
        {
            for(llvm::BasicBlock::iterator inn=BB->begin(), ine=BB->end(); inn!=ine ;++inn)
            {
                errs().write_escaped(inn->getOpcodeName () )<<" ";
                string tmpstr=inn->getOpcodeName ();
                string oper;
                for (User::op_iterator i = inn->op_begin(), e = inn->op_end(); i != e; ++i)
                {
                    Value * v = *i;

                    errs().write_escaped( v->getName())<<" ";
                    oper=v->getName();
                }
                if(tmpstr.compare("call")==0)
                {
                    llvm::Function& insmn=findfunc(M,oper);
                    if(insmn.getName().str().compare("main")!=0)
                    {
                       dumpfheader(insmn);
                                if(!insmn.empty())

                                {
                                    llvm::BasicBlock * IBB = &insmn.getEntryBlock();
                                    Tree(IBB,M);
                                }
                                else
                                {
                                    errs()<<"\nebuchaya huita!\n";
                                }

                    }

                }
                errs().write_escaped(" ")<<"\n";

            }

            TI=BB->getTerminator();
            unsigned int NS= TI->getNumSuccessors();
            for(int i=0;i<NS;i++)
            {
                NBB=TI->getSuccessor(i);
                Tree(NBB,M);
            }
        }

    }
    
    
};
}

char Uir::ID = 0;
static RegisterPass<Uir> X("uir", "uir pass", false, false);
