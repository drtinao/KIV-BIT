aes: main.o encrypt_logic.o io.o resources.h
	gcc main.o encrypt_logic.o io.o -o aes

main.o: main.c main.h
	gcc main.c -c

encrypt_logic.o: encrypt_logic.c encrypt_logic.h
	gcc encrypt_logic.c -c

io.o: io.c io.h
	gcc io.c -c
