
#include "llvm-c/Core.h"
#include "llvm/Pass.h"
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
#include "llvm/Support/raw_ostream.h"
#include "llvm-c/BitWriter.h"
#include "llvm/Support/FileSystem.h"
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>



/*
 * An LLVM Pass that modifies a function in the module IR 
 * by adding arguments to the function.
 */

using namespace std;

using namespace llvm;

namespace {

struct modFuncc : public ModulePass {

 static char ID;
 public:bool flag=false;
 ValueToValueMapTy VMap;
 modFuncc() : ModulePass(ID) {}
 virtual bool runOnModule(Module &m){
		if(flag) return true;
		else{
			flag=true;
			LLVMContext &context = getGlobalContext();
			IRBuilder<> builder(context);

			Module::FunctionListType &functions = m.getFunctionList();
			for (Module::FunctionListType::iterator it = functions.begin(),
                it_end = functions.end(); it != it_end; ++it) {
                Function &F = *it;
				Function *x=cast<Function >(&F);
				Constant* c = m.getOrInsertFunction("mod_add",IntegerType::get(m.getContext(),32),IntegerType::get(m.getContext(),32),IntegerType::get(m.getContext(),32),IntegerType::get(m.getContext(),32),NULL);
  
				Function  *mod_add = cast<Function>(c);
				std::vector<Type*> ArgTypes;
					for (const Argument &I : x->args())
					if (VMap.count(&I) == 0) // Haven't mapped the argument to anything yet?
					ArgTypes.push_back(I.getType());

				    // Loop over the arguments, copying the names of the mapped arguments over...
					for(  Function::arg_iterator DestI = mod_add->arg_begin(),Dest = mod_add->arg_end();DestI!=Dest;++DestI){

  
						for (const Argument & I : x->args())

							if (VMap.count(&I) == 0) {     // Is this argument preserved?
							DestI->setName(I.getName()); // Copy the name over...
							VMap[&I] = &*DestI++;        // Add mapping to VMap
      
						}
					DestI->setName("xtra");

					}


				SmallVector<ReturnInst*, 8> Returns;  // Ignore returns cloned.
				CloneFunctionInto(mod_add, x, VMap, true, Returns, "", 0);
				F.removeFromParent();

				break;
			}
			error_code EC;
			//Write modified module to file named out.bc
			raw_fd_ostream OS("out.bc", EC, sys::fs::F_None);
 
			if (EC)
			return -1;
			WriteBitcodeToFile(&m, OS);
			return true;

		}
	}	
};
}
char modFuncc::ID = 0;
static RegisterPass<modFuncc> Y("mine", "modifies function's args",false,false);	