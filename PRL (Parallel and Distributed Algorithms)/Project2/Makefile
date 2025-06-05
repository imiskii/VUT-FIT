
CC=mpic++
PROG=vuv
CFLAGS=-pedantic -Wall #-Wextra

PROC=1

.PHONY: build run test clean

build:
	$(CC) $(CFLAGS) -o $(PROG) $(PROG).cpp


run: build
	mpirun ./$(PROG) --oversubscribe -np $(PROC) oets


test:
	./test.sh


clean:
	rm -f $(PROG) *.o *.zip