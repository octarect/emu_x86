# Makefile
px86: modrm.o emulator_function.o instruction.o main.c
	cc -o px86 modrm.o emulator_function.o instruction.o main.c

emulator_function.o: emulator_function.h emulator_function.c
	cc -c emulator_function.c

instruction.o: instruction.h instruction.c modrm.o
	cc -c instruction.c

modrm.o: modrm.c
	cc -c modrm.c

clean:
	rm modrm.o
	rm emulator_function.o
	rm instruction.o
	rm px86
