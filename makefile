ifndef $(DESTDIR)
	DESTDIR = /usr/bin
endif

all:
	gcc 10ftwm.c -Wall -std=c11 -ggdb -lxcb -o 10ftwm && ./10ftwm -s :1 
install:
	cp 10ftwm $(DESTDIR)
