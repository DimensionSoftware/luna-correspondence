
CFLAGS  = -O3 -fomit-frame-pointer -funroll-loops -pipe #-DLUNA_DEBUG
OBJ     = src/Receiver.o src/Transmitter.o src/LinkedList.o src/User.o src/Luna.o src/main.o
CC      = gcc -I/usr/local/include -I/sw/include
VERSION = `head -40 src/Luna.h|tail -1|sed -e 's,.*"\(.*\)",\1,'`

luna : $(OBJ)
	gcc $(OBJ) -o $@ -I/usr/local/include -L/usr/local/lib -I/sw/include -L/sw/lib -lpcap
	@echo p00f!  Luna build phase completed on `uname`.
	@echo
	@./configure.pl

clean :
	rm -f src/*.o core *~ luna *.tgz

dist :
	@echo Creating distribution for version $(VERSION)
	@tar zcvf luna-correspondence-$(VERSION).tgz Makefile README INSTALL HACKING Changes configure.pl users/default doc/*.txt doc/*.rfc src/*.[ch]

.c.o :
	$(CC) -o $@ $(CFLAGS) -Wall -c $<


# $Id: Makefile,v 1.12 2002/12/17 14:43:34 keith Exp $
