#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMeffective_address_analysis.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMeffective_address_analysis.so $1
fi
