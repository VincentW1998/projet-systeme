CC=gcc
CFLAGS=-Wall -g
LDLIBS=-lm
all : shell
#all : shell mon_cp mon_rm mon_mv
shell : shell.o gestionnaire.o myCd.o myCat.o myMkdir.o myLs.o check.o tar.o storeRestore.o myRmdir.o redirection.o UnitTest.o myCp.o monrm.o monmv.o pipe.o

monrm.o : monrm.c monrm.h tar.h storeRestore.h check.h myCd.h

shell.o : shell.c gestionnaire.h

gestionnaire.o : gestionnaire.c gestionnaire.h myCd.h redirection.h monmv.h

monmv.o : monmv.c monmv.h monrm.h myCp.h

myCd.o : myCd.c myCd.h gestionnaire.h check.h storeRestore.h

myCat.o : myCat.c myCat.h tar.h storeRestore.h

check.o : check.c check.h tar.h myLs.h

myRmdir.o : myRmdir.c myRmdir.h check.h tar.h myCd.h storeRestore.h

storeRestore.o : storeRestore.c storeRestore.h gestionnaire.h

tar.o : tar.c tar.h

redirection.o : redirection.c redirection.h gestionnaire.h storeRestore.h myCd.h check.h

myMkdir.o : myMkdir.c myMkdir.h check.h tar.h myCd.h storeRestore.h

redirection.o : redirection.c redirection.h myCd.h gestionnaire.h storeRestore.h check.h

myLs.o : myLs.c myLs.h tar.h check.h storeRestore.h

myCp.o : myCp.c myCp.h check.h tar.h myCd.h storeRestore.h

UnitTest.o : UnitTest.c UnitTest.h myCd.h storeRestore.h

pipe.o : pipe.c pipe.h gestionnaire.h

clean :
	rm -rf *.o

cleanall : clean
	rm -rf shell mon_cp mon_rm mon_mv *.dSYM/ *.gch
