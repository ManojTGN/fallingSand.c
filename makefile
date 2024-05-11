CC = gcc
BINS = fallingSand

all:$(BINS)

fallingSand:
	if [ ! -d build ]; then mkdir build; fi
	$(CC) fallingSand.c -o build/fallingSand.exe 
