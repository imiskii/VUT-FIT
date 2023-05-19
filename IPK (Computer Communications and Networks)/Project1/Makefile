
CC=g++
CFLAGS=-std=c++20
LIBS=-lm -lstdc++fs -lpcap

PROGS=ipkcpc
OBJ=parseArg.o errors.o ipkcpc_tcp.o ipkcpc_udp.o ipkcpc.o

.PHONY: clean zip

all: $(PROGS)
	

#######################################################################

$(PROGS): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


%.o: %.cpp %.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

#######################################################################



tcp:
	./ipkcpc -h 127.0.0.1 -p 2023 -m tcp


udp:
	./ipkcpc -h 127.0.0.1 -p 2023 -m udp


#######################################################################


zip:
	git archive --format=zip HEAD -o xlasmi00.zip


clean:
	rm -f $(PROGS) *.o *.zip
