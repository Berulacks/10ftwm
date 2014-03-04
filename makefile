all:
	gcc 10ftwm.c -Wall -ggdb -lxcb -o runfile && ./runfile 
