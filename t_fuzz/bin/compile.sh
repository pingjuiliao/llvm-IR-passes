#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMt_fuzz.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMt_fuzz.so $1
fi
