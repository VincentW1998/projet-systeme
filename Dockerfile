FROM ubuntu:latest
RUN apt update
RUN apt-get install -y vim
RUN apt-get install -y libc-dev
RUN apt-get install -y gcc
RUN apt-get install -y make
RUN mkdir /home/projet
COPY code/Makefile /home/projet/Makefile
COPY code/shell.c /home/projet/shell.c
COPY code/gestionnaire.c /home/projet/gestionnaire.c
COPY code/gestionnaire.h /home/projet/gestionnaire.h
COPY code/tarball.h /hom/projet/tarball.h
