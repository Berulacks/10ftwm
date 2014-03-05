all:
	gcc 10ftwm.c -Wall -std=c99 -ggdb -lxcb -o runfile && ./runfile 
