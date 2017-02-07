server: server.o helper.o
	gcc -o server server.o helper.o -Wall

server.o: server.c helper.h
	gcc -o server.o server.c -c -ansi -pedantic -Wall

helper.o: helper.c helper.h
	gcc -o helper.o helper.c -c -ansi -pedantic -Wall

clean:
	rm helper.o server.o server








