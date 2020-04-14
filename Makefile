CC=g++
CFLAGS=-I.

all: main.cpp json11.cpp
	$(CC) -lpaho-mqtt3c -o replayer main.cpp json11.cpp
