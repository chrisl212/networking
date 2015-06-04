CC=gcc
CFLAGS=-c -lpthread

all: http_download.h http_download.c
	$(CC) $(CFLAGS) http_download.c -o http_download.o
	ar rvs libclnet.a http_download.o
	mv libclnet.a /usr/lib/libclnet.a
	mkdir -p /usr/include/clnet/
	cp *.h /usr/include/clnet/
	rm http_download.o
