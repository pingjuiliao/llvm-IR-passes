#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMsplitBasicBlock.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMsplitBasicBlock.so $1
fi
