FROM ubuntu:latest
RUN apt update
RUN apt upgrade -y
RUN apt-get install -y vim
RUN apt-get install -y libc-dev
RUN apt-get install -y gcc
RUN apt-get install -y make
RUN apt-get install -y gdb
RUN apt-get install -y tree
RUN apt-get install -y emacs
RUN mkdir /home/projet
COPY code/*.h /home/projet/
COPY code/*.c /home/projet/
COPY code/essai.tar /home/projet/essai.tar
COPY code/Makefile /home/projet/Makefile
