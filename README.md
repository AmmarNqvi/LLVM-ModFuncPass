# LLVM-Pass
Transformation Passes for LLVM IR

The BlockCountPass.cpp is a test pass that counts function blocks in each function in the module.

The func_mod.cpp is a pass that modifies the module;replacing the first function with a new modified version with an extra integer argument.

The test.c file is the test case file used, the output of a successful run should look like:
" 
  hello world
  8
"

To run the pass

-Convert .c file to bitcode format
  clang -O3 -emit-llvm test.c -c -o test.bc
  
  //****  for the function modifier pass  ****//
  
-Create a .o file to generate a shared library file for our pass
  clang++ -c func_mod.cpp `llvm-config --cxxflags` //for the function modifier pass
  
-Create shared library file
  clang++ -shared -o pass.so func_mod.o `llvm-config --ldflags`
  
-Run the pass using the LLVM opt tool
  opt -load ./pass.so -mine < o.bc > /dev/null
  
There should be a new bitcode file created called "out.bc"
Run the bitcode file using lli tool; lli out.bc


