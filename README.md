# My LLVM pass projects

## Projects I built
| Project    | Description                                            |       status       |
|------------|--------------------------------------------------------|--------------------|
| Stackguard | mimic stackguard                                       | only push constant |
| T-fuzz     | mimic T-fuzz                                           | flip the branch    |
| Cmp reduce | divide n-bytes comparason into byte-by-byte comparason | int32 cmp completed|
| -----------| -----------------|



## To build new pass project
```
    $ cp -r ./template ./new_llvm_pass_project
    $ ## then edit the file pass.cpp and compile with compile.sh
```

## To compile & run
```
    $ cd new_llvm_pass_project/bin
    $ cmake ..
    $ make
    $ ./compile.sh ## compile and shows the compile-time data
    $ ./a.out  ## Transformed program !!!

```
