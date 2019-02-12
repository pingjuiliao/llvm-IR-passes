#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMprint_func.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMprint_func.so $1
fi
