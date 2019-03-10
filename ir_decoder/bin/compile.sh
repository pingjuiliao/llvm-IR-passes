#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMir_decoder.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMir_decoder.so $1
fi
