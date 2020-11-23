CC=gcc
CFLAGS=-Wall -g
LDLIBS=-lm
all : shell mon_cp mon_rm mon_mv

shell : shell.o gestionnaire.o my_cd.o my_cat.o

shell.o : shell.c gestionnaire.h

gestionnaire.o : gestionnaire.c gestionnaire.h

my_cd.o : my_cd.c my_cd.h

my_cat.o : my_cat.c my_cat.h

mon_cp : mon_cp.c

mon_rm : mon_rm.c

mon_mv : mon_mv.c

clean :
	rm -rf *.o

cleanall : clean
	rm -rf shell mon_cp mon_rm mon_mv *.dSYM/ *.gch
