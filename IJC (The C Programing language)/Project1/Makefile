
#  Makefile
#  Author: Michal Ľaš


CC=gcc
CFLAGS=-O2 -std=c11 -pedantic -Wall -Wextra
PROGS=steg-decode primes primes-i


all: $(PROGS)

primes: primes.o eratosthenes.o error.o bitset.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

primes-i: primes-i.o eratosthenes-i.o error.o bitset-i.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

steg-decode: eratosthenes.o error.o bitset.o ppm.o steg-decode.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

############################## PRIMES ###################################

# MACROS #

primes.o: primes.c
	$(CC) $(CFLAGS) -c $< -o $@

eratosthenes.o: eratosthenes.c
	$(CC) $(CFLAGS) -c $< -o $@

bitset.o: bitset.c
	$(CC) $(CFLAGS) -c $< -o $@

# INLINE FUNCTIONS #

primes-i.o: primes.c
	$(CC) $(CFLAGS) -DUSE_INLINE -c $< -o $@

eratosthenes-i.o: eratosthenes.c
	$(CC) $(CFLAGS) -DUSE_INLINE -c $< -o $@

bitset-i.o: bitset.c
	$(CC) $(CFLAGS) -DUSE_INLINE -c $< -o $@

############################ STEG-DECODE ###################################

ppm.o: ppm.c
	$(CC) $(CFLAGS) -c $<  -o $@

error.o: error.c
	$(CC) $(CFLAGS) -c $< -o $@

steg-decode.o: steg-decode.c
	$(CC) $(CFLAGS) -c $< -o $@

############################### CLEAN ######################################

clean:
	rm -f *.o
	rm -f $(PROGS)

############################ RUN AND ZIP ###################################

run:
	ulimit -s 40000 && ./primes && ./primes-i; 

zip: *.c *.h Makefile
	zip xlasmi00.zip *.c *.h Makefile