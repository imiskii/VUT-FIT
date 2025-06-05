# FLP - Rubik's cube solver
# Michal Ľaš (xlasmi00)
# 11.04.2025
# Makefile

PROG=flp24-log

PYTHON=python3
PROLOG=swipl
FLAGS=-q

SRCDIR=.
TESTDIR=./tests

TOPACK=Makefile $(SRCDIR)/*.pl README.md $(TESTDIR)


build:
	$(PROLOG) $(FLAGS) -g start -o $(PROG) -c $(SRCDIR)/rubiks_cube.pl


alt:
	$(PROLOG) $(FLAGS) --stack_limit=16g -g start -o $(PROG) -c $(SRCDIR)/rubiks_cube_alt.pl


test:
	$(PYTHON) $(TESTDIR)/test.py ./$(PROG) $(TESTDIR)/inputs


pack: clean
	zip -r flp-log-xlasmi00 $(TOPACK) 


clean:
	rm -rf *.o $(PROG) *.zip