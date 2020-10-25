CC=gcc
CFLAGS=-Wall -g
LDLIBS=-lm
all : shell mon_cp mon_rm mon_mv

shell : shell.o gestionnaire.o

shell.o : shell.c gestionnaire.h

gestionnaire.o : gestionnaire.c gestionnaire.h

mon_cp : mon_cp.c

mon_rm : mon_rm.c

mon_mv : mon_mv.c

clean :
	rm -rf *.o

cleanall : clean
	rm -rf shell mon_cp mon_rm mon_mv
