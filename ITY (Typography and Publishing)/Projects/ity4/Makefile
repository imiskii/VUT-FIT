#
# Projekt: proj4.tex
# Autor:   Michal Ľaš
# Datum:   11.04.2023
# 


NAME=proj4

all: proj

proj: $(NAME).tex
	latex $(NAME).tex
	make bibtex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t -a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps



bibtex: $(NAME).bib
	bibtex $(NAME)


zip:
	zip xlasmi00.zip Makefile $(NAME).tex $(NAME).bib czechiso.bst


clean:
	rm -f *.dvi
	rm -f *.ps
	rm -f *.aux
	rm -f *.out
	rm -f *.log
	rm -f *.bbl *.blg *.bcf *.run.xml
	rm -f *.gz
	rm $(NAME).pdf
