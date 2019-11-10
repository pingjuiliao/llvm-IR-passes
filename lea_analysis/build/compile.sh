#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMlea_analysis.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMlea_analysis.so $1
fi
