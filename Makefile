all: tcpget

tcpget: src/tcpget.c
	mkdir -p build
	cc -g -O0 -Wall -Wextra -o build/tcpget src/tcpget.c
