all: tcpget echod

tcpget: src/tcpget.c
	cc -g -O0 -Wall -Wextra -o build/tcpget src/tcpget.c

echod: src/echod.c
	cc -g -O0 -Wall -Wextra -o build/echod src/echod.c
