all:server
server:server.cpp wrap.c
g++ $< -o $@

clean:
	rm -f server
