#
# Project: dhcp-stats
# Author:  Michal Ľaš (xlasmi00)
# Date:    19.10.2023
# 

CC=g++
CFLAGS=-std=c++20 -I. 
LIBS=-lncurses -lstdc++fs -lpcap -pthread

LOGIN=xlasmi00

BIN=./bin/
HDR_FILES := $(wildcard *.hpp)
SRC_FILES := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp,$(BIN)/%.o,$(SRC_FILES))
DEP := $(OBJ:.o=.d)

TOPACK= ../src/*.cpp ../src/*.hpp ../src/Makefile ../doc/images ../doc/manual.pdf ../README.md ../dhcp-stats.1

PROGS=dhcp-stats
OBJ=$(BIN)dhcp-stats.o $(BIN)argparse.o $(BIN)errors.o $(BIN)ip-prefix.o $(BIN)dhcp-monitor.o $(BIN)consol-log.o 

.PHONY: clean all pack


all: $(BIN) $(PROGS)


#######################################################################


$(PROGS): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)


$(BIN)%.o: %.cpp %.hpp | $(BIN)
	$(CC) -c $(CFLAGS) -o $@ $< 


-include $(DEP)


$(BIN):
	mkdir -p $(BIN)


#######################################################################


pack: clean
	tar -cvf $(LOGIN).tar $(TOPACK)


pack2: clean
	tar -cvf $(LOGIN).tar $(TOPACK) ../test_pcap_files


clean:
	rm -rf $(BIN) $(PROGS) $(wildcard $(DEP)) *.tar