all: server

server: http_server_with_thread_pool.c
	gcc http_server_with_thread_pool.c -o server -g -lpthread

.PHONY: clean

clean:
	rm -rf server a.out
