#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMglobvar.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMglobvar.so $1
fi
