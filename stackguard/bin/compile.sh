#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMstackguard.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMstackguard.so $1
fi
