# Makefile
px86: main.c emulator_function.o instruction.o
	cc -o px86 emulator_function.o instruction.o main.c

emulator_function.o: emulator_function.h emulator_function.c
	cc -c emulator_function.c

instruction.o: instruction.h instruction.c
	cc -c instruction.c

clean:
	rm emulator_function.o
	rm instruction.o
	rm px86
