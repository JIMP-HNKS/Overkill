liboverkill.a: overkill.o
	ar rcs lib/$@ $(addprefix build/,$^)

overkill.o:
	gcc -c -o build/$@ src/overkill.c -ggdb