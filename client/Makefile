client: client.o
	gcc -o client client.o -Wall

client.o: client.c
	gcc -o client.o client.c -c -ansi -pedantic -Wall -D_BSD_SOURCE

clean:
	rm client.o client
