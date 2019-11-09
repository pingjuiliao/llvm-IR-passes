CC=clang
IR= -Xclang -load -Xclang ../libLLVMir_decoder.so 
charcmp: charcmp.c
	$(CC) -o charcmp $(IR) charcmp.c
intcmp: intcmp.c
	$(CC) -o intcmp $(IR) intcmp.c
global_variable: global_variable.c
	$(CC) -o global_variable $(IR) global_variable.c
clean:
	rm global_variable intcmp charcmp

