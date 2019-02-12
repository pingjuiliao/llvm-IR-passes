#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMget_IR.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMget_IR.so $1
fi
