CFLAGS=-c -Wall -g -std=c++11


client: server client.o
	c++  client.o sock_wrap.o -o client

server: dbms.o sock_wrap.o inter.o server.o
	c++  server.o sock_wrap.o dbms.o inter.o -o server

inter.o: inter.cpp dbms.o
	c++ $(CFLAGS) inter.cpp -o inter.o

dbms.o: dbms.cpp sock_wrap.o
	c++ $(CFLAGS) dbms.cpp -o dbms.o

sock_wrap.o: sock_wrap.cpp server.o
	c++ $(CFLAGS) sock_wrap.cpp -o sock_wrap.o

server.o: server.cpp client.o
	c++ $(CFLAGS) server.cpp -o server.o

client.o: client.cpp clean
	c++ $(CFLAGS) client.cpp -o client.o
clean:
	rm -rf *.o *.dSYM server client || true 	
