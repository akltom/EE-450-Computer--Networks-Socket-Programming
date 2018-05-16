client: client.c
	gcc -o client -g -lsocket client.c -lnsl -lresolv

server: server.o fork.o
	gcc -o server -g server.o fork.o -lsocket -lnsl -lresolv

server.o: server.c
	gcc -g -c server.c

fork.o: fork.c
	gcc -g -c fork.c

clean:
	rm -f *.o client server
