#
# Project: manual.tex
# Author: Michal Ľaš
# Date: 19.10.2023
# 


NAME=manual

all: proj

proj: $(NAME).tex
	pdflatex $(NAME).tex
	make bibtex 
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex

	mv ./manual.pdf ../



bibtex: $(NAME).bib
	bibtex $(NAME)


zip:
	zip xlasmi00.zip Makefile $(NAME).tex $(NAME).bib enplain.bst


clean:
	rm -f *.dvi
	rm -f *.ps
	rm -f *.aux
	rm -f *.out
	rm -f *.log
	rm -f *.bbl *.blg *.bcf *.run.xml
	rm -f *.gz
	rm -f *.toc
#	rm ../$(NAME).pdf
