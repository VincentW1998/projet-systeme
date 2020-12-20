FROM ubuntu:latest
RUN apt update
RUN apt-get install -y vim
RUN apt-get install -y libc-dev
RUN apt-get install -y gcc
RUN apt-get install -y make
RUN apt-get install -y gdb
RUN mkdir /home/projet
COPY code/Makefile /home/projet/Makefile
COPY code/shell.c /home/projet/shell.c
COPY code/gestionnaire.c /home/projet/gestionnaire.c
COPY code/gestionnaire.h /home/projet/gestionnaire.h
COPY code/myCd.h /home/projet/myCd.h
COPY code/myCd.c /home/projet/myCd.c
COPY code/myCat.h /home/projet/myCat.h
COPY code/myCat.c /home/projet/myCat.c
COPY code/myLs.h /home/projet/myLs.h
COPY code/myLs.c /home/projet/myLs.c
COPY code/tar.h /home/projet/tar.h
COPY code/tar.c /home/projet/tar.c

COPY code/mon_cp.c /home/projet/mon_cp.c
COPY code/mon_rm.c /home/projet/mon_rm.c
COPY code/mon_mv.c /home/projet/mon_mv.c
