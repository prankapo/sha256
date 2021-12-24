run: sha256.out
	./sha256.out

sha256.out: sha256.o
	gcc sha256.o -lm -o sha256.out
sha256.o: sha256.c
	gcc -c sha256.c
