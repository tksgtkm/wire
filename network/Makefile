all: http

OPTS = -g -Wall -Wno-pointer-sign

http: http.c base64.c base64.h
	gcc -o http $(OPTS) http.c base64.c

clean:
	rm -f http
	rm -f core
	rm -f core.*