ifndef $(DESTDIR)
	DESTDIR = /usr/bin
endif

all:
	gcc 10ftwm.c -Wall -std=c11 -ggdb -lxcb -o 10ftwm
test:
	gcc 10ftwm.c -Wall -std=c11 -ggdb -lxcb -o 10ftwm.test && ./10ftwm.test -s :1 ; rm 10ftwm.test
install:
	cp 10ftwm $(DESTDIR)
