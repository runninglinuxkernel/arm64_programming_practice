COPS = -g -march=armv8-a+sve

sve_strcmp: sve_strcmp.o strcmp_asm.o
	gcc $(COPS) -o sve_strcmp sve_strcmp.o strcmp_asm.o

sve_strcmp.o:sve_strcmp.c
	gcc -c sve_strcmp.c $(COPS)
	
strcmp_asm.o:strcmp_asm.S
	gcc -c strcmp_asm.S $(COPS)

clean:
	rm *.o
	rm sve_strcmp
