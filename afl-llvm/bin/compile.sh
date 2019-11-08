#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMafl-llvm.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMafl-llvm.so $1
fi
