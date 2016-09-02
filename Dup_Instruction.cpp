
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm-c/Core.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm-c/BitWriter.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Analysis/CFG.h"
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

/*
 * An LLVM Pass that replaces an add instruction in the module IR 
 * with a new myAdd instruction.
 */

namespace {
  struct NewInst : public ModulePass {
    static char ID;
    NewInst() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) {
		//Create new Add instruction named test and mask params to 42 and -42
		auto& ctx = getGlobalContext();
		auto myAdd = BinaryOperator::Create(Instruction::Add,
     		ConstantInt::get(cast<Type>(Type::getInt32Ty(ctx)), APInt(32, 42)),
     		ConstantInt::get(cast<Type>(Type::getInt32Ty(ctx)), APInt(32, -42)),"test");
		
        for (auto& F : M) {
            
            for (auto& BB : F) {
                
                for (auto& I : BB) {
                    
					//Iterate through instructions of each function in the module
					
					if(strcmp(I.getOpcodeName(),"add")==0)
					{
						auto &oldAdd=I;
						ReplaceInstWithInst(&oldAdd, myAdd);
						oldAdd.replaceAllUsesWith(myAdd);
						break;
					}	
                }
            }
        }
		
		error_code EC;
		raw_fd_ostream OS("obf.bc", EC, sys::fs::F_None);
 
		if (EC)
		return -1;
		WriteBitcodeToFile(&M, OS);
        return false;
    }
  };
}

char NewInst::ID = 0;

// Automatically enable the pass.
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new NewInst());
}

static RegisterPass<NewInst> Y("inst", "obfuscates code by ",false,false);