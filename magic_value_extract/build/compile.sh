#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMmagic_value_extract.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMmagic_value_extract.so $1
fi
