# My LLVM pass projects

## The projects I built

### Stackguard : Stack canary
###### This implementation is without isolation, it only pushes constant on the stack 

### t_fuzz     : change the branch condition so that the 
###### This implementation reverse all branch

### cmp_reduce : change multibytes comparason to byte-by-byte comparason
###### On progress 


## To build new pass project
```
    $ cp -r ./template ./new_llvm_pass_project
    $ chmod +w ./new_llvm_pass_project
    $ ## then edit the file pass.cpp and compile with compile.sh
```

## To compile & run
```
    $ cd new_llvm_pass_project/bin
    $ cmake ..
    $ make
    $ ./run.sh ## compile and shows the compile-time data
    $ ./a.out  ## Transformed program !!!

```
