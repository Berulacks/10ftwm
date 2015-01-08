ifndef $(DESTDIR)
	DESTDIR = /usr/bin
endif

all:
	gcc 10ftwm.c -Wall -std=c11 -ggdb -lxcb -llirc_client -o 10ftwm
test:
	gcc 10ftwm.c -Wall -std=c11 -ggdb -lxcb -llirc_client -o 10ftwm.test && DISPLAY=:1 ./10ftwm.test -d :1 ; rm 10ftwm.test
install:
	cp 10ftwm $(DESTDIR)
clean:
	rm -f *.test 10ftwm
