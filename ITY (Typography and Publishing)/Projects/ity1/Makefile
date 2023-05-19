#
# Projekt: proj1.tex
# Autor:   Michal Ľaš
# Datum:   05.03.2023
# 


NAME=proj1

all: proj

proj: proj1.tex
	latex $(NAME).tex
	dvips -t -a4 $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm -f *.dvi
	rm -f *.ps
	rm -f *.aux
	rm -f *.out
	rm -f *log
	rm $(NAME).pdf