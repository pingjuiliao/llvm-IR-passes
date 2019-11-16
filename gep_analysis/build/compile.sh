#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMgep_analysis.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMgep_analysis.so $1
fi
