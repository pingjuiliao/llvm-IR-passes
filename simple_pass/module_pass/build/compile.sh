#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMmodule_pass.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMmodule_pass.so $1
fi
