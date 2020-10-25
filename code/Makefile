CC=gcc
CFLAGS=-Wall -g
LDLIBS=-lm
all : shell

shell : shell.o gestionnaire.o

shell.o : shell.c gestionnaire.h

gestionnaire.o : gestionnaire.c gestionnaire.h

clean :
	rm -rf *.o

cleanall : clean
	rm -rf shell
