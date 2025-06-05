# FLP - Decision Tree
# Michal Ľaš (xlasmi00)
# 09.03.2025
# Makefile

PROG=flp-fun
HASKELL=ghc
FLAGS=-Wall

BINDIR=./bin
SRCDIR=.
TESTDIR=./tests

TOPACK=Makefile $(SRCDIR)/*.hs VERSION TEST_OUTPUT_1 TEST_OUTPUT_2

merlin_libncursesw_path=/lib/x86_64-linux-gnu/libncursesw.so.6.4


build:
	$(HASKELL) $(SRCDIR)/Main.hs $(FLAGS) --make -o $(PROG) -i$(SRCDIR) -outputdir .


build-merlin:
	mkdir -p ./libs
	ln -s $(merlin_libncursesw_path) ./libs/libncursesw.so.5
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):./libs make build
	rm -rf ./libs


pack:
	zip -r xlasmi00 $(TOPACK) 

clean:
	rm -rf *.o *.hi $(PROG) *.zip
