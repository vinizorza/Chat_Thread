all:METODOS
	gcc -pthread server.c -o server
	gcc -pthread client.c -o client
METODOS:
	gcc -c server.c
	gcc -c client.c
