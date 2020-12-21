CC=gcc
CFLAGS=-Wall -g
LDLIBS=-lm
all : shell
#all : shell mon_cp mon_rm mon_mv
shell : shell.o gestionnaire.o myCd.o myCat.o myMkdir.o myLs.o check.o tar.o

shell.o : shell.c gestionnaire.h

gestionnaire.o : gestionnaire.c gestionnaire.h myCd.h

myCd.o : myCd.c myCd.h gestionnaire.h check.h

myCat.o : myCat.c myCat.h tar.h

check.o : check.c check.h tar.h

tar.o : tar.c tar.h

myMkdir.o : myMkdir.c myMkdir.h check.h tar.h myCd.h

myLs.o : myLs.c myLs.h tar.h
#mon_cp : mon_cp.c
#
#mon_rm : mon_rm.c
#
#mon_mv : mon_mv.c

clean :
	rm -rf *.o

cleanall : clean
	rm -rf shell mon_cp mon_rm mon_mv *.dSYM/ *.gch
