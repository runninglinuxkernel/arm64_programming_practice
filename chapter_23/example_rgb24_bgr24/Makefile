COPS = -g -march=armv8-a+sve

rgb24_bgr24_sve: rgb24_bgr24_sve.o rgb24_bgr24_asm.o
	gcc $(COPS) -o rgb24_bgr24_sve rgb24_bgr24_sve.o rgb24_bgr24_asm.o

rgb24_bgr24_sve.o:rgb24_bgr24_sve.c
	gcc -c rgb24_bgr24_sve.c $(COPS)
	
rgb24_bgr24_asm.o:rgb24_bgr24_asm.S
	gcc -c rgb24_bgr24_asm.S $(COPS)

clean:
	rm *.o
	rm rgb24_bgr24_sve
