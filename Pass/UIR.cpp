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
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/LibCallSemantics.h"
#include <string>
#include "llvm/IR/Constants.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Utils/SimplifyLibCalls.h"
#include "llvm/Analysis/LibCallSemantics.h"
//#include "llvm/Support/DebugLoc.h"
using namespace std;
using namespace llvm;

namespace {
struct Uir : public ModulePass {
    static char ID;
    Uir() : ModulePass(ID) {}
    std::set< BlockAddress* > MasBA;
    string  TID[20] = {
        "VoidTyID" , "HalfTyID", "FloatTyID", "DoubleTyID",
        "X86_FP80TyID", "FP128TyID", "PPC_FP128TyID", "LabelTyID",
        "MetadataTyID", "X86_MMXTyID", "IntegerTyID", "FunctionTyID",
        "StructTyID", "ArrayTyID", "PointerTyID", "VectorTyID",
        "NumTypeIDs", "LastPrimitiveTyID = X86_MMXTyID", "FirstDerivedTyID = IntegerTyID"
    };

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


        llvm::Function * mn=M.getFunction("main");

        dumpfheader(*mn);
        if(!mn->empty())
        {
            llvm::BasicBlock * BB = &mn->getEntryBlock();
            Tree(BB,M);
        }
        return false;
    }
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
                string oper;
                Instruction * instr=inn;
                llvm::CallInst * cins=dyn_cast_or_null<CallInst>(instr);
                if(cins==NULL)
                {
                    errs()<<"--";
                }
                else
                {
                    for (User::op_iterator i = inn->op_begin(), e = inn->op_end(); i != e; ++i)
                    {
                        Value * v = *i;

                        errs().write_escaped( v->getName())<<" ";
                        oper=v->getName();
                    }

                    llvm::Function* insmn=cins->getCalledFunction();

                    if(insmn==NULL)
                    {
                        llvm::LibCallSimplifier * LCS;
                        Value * Val = LCS->optimizeCall(cins);
                        errs()<<"Notfound";
                        llvm::CallInst * cins2=dyn_cast_or_null<CallInst>(Val);

                    }
                    else
                    {
                        dumpfheader(*insmn);
                        if(!insmn->empty())

                        {
                            llvm::BasicBlock * IBB = &insmn->getEntryBlock();
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
