#
# Projekt: ipk-l4-scan
# Autor:   Michal Ľaš
# Datum:   05.03.2023
# 

CC=g++
CFLAGS=-std=c++20 -I. 
LIBS=-lm -lstdc++fs -lpcap

PROGS=ipk-l4-scan
OBJ=nscan.o argparse.o errors.o nscan-tcp.o nscan-udp.o headers.o

.PHONY: clean


all: $(PROGS)


#######################################################################

$(PROGS): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

#######################################################################


zip:
	git archive --format=zip HEAD -o xlasmi00.zip


clean:
	rm -f $(PROGS) *.o *.zip