CC=gcc
BIN_DIR="bin"
all: prereq setup-custom-baudrate

prereq:
ifeq (,$(wildcard bin))
	mkdir bin
endif

setup-custom-baudrate : src/setup-custom-baudrate.c
	gcc src/$@.c -o bin/$@ 


