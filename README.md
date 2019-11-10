# My LLVM pass projects

## Projects I built
### binary instrumentation
|    Project             | Description                                            |       status       |
|------------------------|--------------------------------------------------------|--------------------|
|  [StackGuard]          | mimic stackguard                                       | only push constant |
|   [T-fuzz]             | mimic T-fuzz                                           | flip the branch    |
|  [Cmp reduce]          | divide n-bytes comparason into byte-by-byte comparason | int32 cmp completed|

### program analysis
|    Project             | Description                                            |       status       |
|------------------------|--------------------------------------------------------|--------------------|
| [Magic Value Analysis] | track   cmp instruction to get magic bytes             | Equality handled() |
| [LEA Analysis]         | track if lea instruction is tainted                    | Might-alias level  |





## Recap for myself
### To build new pass project
```
    $ cp -r ./template ./new_llvm_pass_project
    $ ## then edit the file pass.cpp and compile with compile.sh
```

### To compile & run
```
    $ cd new_llvm_pass_project/bin
    $ cmake ..
    $ make
    $ ./compile.sh ## compile and shows the compile-time data
    $ ./a.out  ## Transformed program !!!

```


[StackGuard]: /stackguard
[T-fuzz]: /t_fuzz
[Cmp reduce]: /cmp_reduce
[Magic Value Analysis]: /magic_value_extract
[LEA Analysis]: /lea_analysis
