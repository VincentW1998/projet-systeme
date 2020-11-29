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
COPY code/my_cd.h /home/projet/my_cd.h
COPY code/my_cd.c /home/projet/my_cd.c
COPY code/tar.h /home/projet/tar.h
COPY code/tar.c /home/projet/tar.c
COPY code/my_cat.c /home/projet/my_cat.c
COPY code/my_cat.h /home/projet/my_cat.h
COPY code/mon_cp.c /home/projet/mon_cp.c
COPY code/mon_rm.c /home/projet/mon_rm.c
COPY code/mon_mv.c /home/projet/mon_mv.c

