#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMcmp_reduce.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMcmp_reduce.so $1
fi
