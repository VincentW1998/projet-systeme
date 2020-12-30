FROM ubuntu:latest
RUN apt update
RUN apt-get install -y vim
RUN apt-get install -y libc-dev
RUN apt-get install -y gcc
RUN apt-get install -y make
RUN apt-get install -y gdb
RUN mkdir /home/projet
COPY code/*.h /home/projet/
COPY code/*.c /home/projet/
COPY code/unitTest /home/projet/unitTest
COPY code/Makefile /home/projet/Makefile
