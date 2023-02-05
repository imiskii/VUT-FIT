
#  Makefile
#  Autor: Michal Ľaš


CC=gcc
CFLAGS=-pthread -std=gnu99 -Wall -Wextra -Werror -pedantic
PROGS=proj2


all: $(PROGS)

proj2: proj2.o h2o.o h2o.h
	$(CC) $(CFLAGS) $^ -o $@

################################ MAIN ######################################

proj2.o: proj2.c
	$(CC) $(CFLAGS) -c $< -o $@

h2o.o: h2o.c
	$(CC) $(CFLAGS) -c $< -o $@


############################### CLEAN ######################################

clean:
	rm -f *.o *.out
	rm -f $(PROGS)

############################ RUN AND ZIP ###################################

run:
	./proj2 3 5 100 100
	./kontrola-vystupu.sh <proj2.out

zip: *.c *.h Makefile
	zip proj2.zip *.c *.h Makefile
