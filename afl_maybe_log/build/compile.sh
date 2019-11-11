#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMafl_maybe_log.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMafl_maybe_log.so $1
fi
