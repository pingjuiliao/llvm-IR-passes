# My LLVM pass projects

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
