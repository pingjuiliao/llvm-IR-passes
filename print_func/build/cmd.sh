#!/bin/bash
if [[ $1 = "" ]] ; then 
    echo "Usage: $0 <library_name>"
else
    clang -Xclang -load -Xclang ./$1 ./fibonacci.c
fi