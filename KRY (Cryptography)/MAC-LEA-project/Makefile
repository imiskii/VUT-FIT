#
# Projekt: kry
# Autor:   Michal Ľaš
# Datum:   27.04.2024
# 

CC=g++
CXXFLAGS=-std=c++20 -fsanitize=address,leak -O2 -pedantic -Wall -Wextra -I. 
LIBS=-static-libasan -fno-omit-frame-pointer -lm

PROGS=kry
OBJ=kry.o argparser.o utils.o sha256.o lea.o

.PHONY: all clean zip


all: $(PROGS)


#######################################################################

$(PROGS): $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LIBS)


%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CXXFLAGS)

#######################################################################


zip:
	zip 241520.zip *.cpp *.hpp Makefile README.md


clean:
	rm -f $(PROGS) *.o *.zip