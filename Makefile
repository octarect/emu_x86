# Makefile

binary_name = program

px86: modrm.o emulator_function.o instruction.o main.c
	cc -o px86 modrm.o emulator_function.o instruction.o main.c
	rm modrm.o emulator_function.o instruction.o
emulator_function.o: emulator_function.h emulator_function.c
	cc -c emulator_function.c
instruction.o: instruction.h instruction.c modrm.o
	cc -c instruction.c
modrm.o: modrm.c
	cc -c modrm.c

test34:
	nasm -o $(binary_name) test/test34.asm

test37_call:
	nasm -o $(binary_name) test/test37_call.asm

test37_cfunc: test37_main.o test37_crt0.o
	ld --entry=start --oformat=binary -Ttext 0x7c00 -m elf_i386 -o $(binary_name) test37_crt0.o test37_main.o
	rm test37_main.o test37_crt0.o
test37_main.o: test/test37_main.c
	cc -nostdlib -fno-asynchronous-unwind-tables -fno-stack-protector -m32 -o test37_main.o -c test/test37_main.c
test37_crt0.o: test/test37_crt0.asm
	nasm -f elf -o test37_crt0.o test/test37_crt0.asm

test37_args: test37_args.o test37_crt0.o
	ld --entry=start --oformat=binary -Ttext 0x7c00 -m elf_i386 -o $(binary_name) test37_crt0.o test37_args.o
	rm test37_args.o test37_crt0.o
test37_args.o: test/test37_args.c
	cc -nostdlib -fno-asynchronous-unwind-tables -fno-stack-protector -m32 -o test37_args.o -c test/test37_args.c

test310: test310.o test37_crt0.o
	ld --entry=start --oformat=binary -Ttext 0x7c00 -m elf_i386 -o $(binary_name) test37_crt0.o test310.o
	rm test310.o test37_crt0.o
test310.o: test/test310.c
	cc -nostdlib -fno-asynchronous-unwind-tables -fno-stack-protector -m32 -o test310.o -c test/test310.c



clean:
	rm px86
	rm $(binary_name)
