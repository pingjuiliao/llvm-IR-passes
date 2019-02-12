#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMonly_operator_multiply.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMonly_operator_multiply.so $1
fi
