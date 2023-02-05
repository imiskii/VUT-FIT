
#  Makefile
#  Author: Michal Ľaš

CC=gcc
CFLAGS=-O2 -std=c11 -pedantic -Wall -Wextra
PROGS=tail libhtab.a libhtab.so wordcount wordcount-dynamic
CDEP=$(shell ls htab_*.c)
ODEP=$(CDEP:.c=.o)
DEP=dependencies

all: $(PROGS)

dependencies:
	$(CC) -MM *.c > $(DEP)

-include $(DEP)

################################ TAIL ######################################

tail: tail.o
	$(CC) $(CFLAGS) $^ -o $@

tail.o: tail.c
	$(CC) $(CFLAGS) -c $< -o $@

############################## WORDCOUNT ###################################

wordcount: wordcount.o io.o libhtab.a
	$(CC) $(CFLAGS) -static $^ -o $@

wordcount-dynamic: wordcount.o io.o libhtab.so
	$(CC) $(CFLAGS) $^ -o $@

wordcount.o: wordcount.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $< htab.h htab_item.h

libhtab.a: $(ODEP)
	ar rcs $@ $^

libhtab.so: $(ODEP)
	$(CC) $(CFLAGS) -shared -fPIC $^ -o $@

############################# READ_WORD ####################################

io: io.o
	$(CC) $(CFLAGS) $^ -o $@

io.o: io.c io.h
	$(CC) $(CFLAGS) -c $< -o $@

############################### CLEAN ######################################

clean:
	rm -f *.o
	rm -f *.gch
	rm -f $(PROGS)
	rm -f $(DEP)

################################ ZIP #######################################

zip: *.c *.h Makefile
	zip xlasmi00.zip *.c *.h Makefile