#
# Projekt: proj5.tex
# Autor:   Michal Ľaš
# Datum:   19.04.2023
# 


NAME=proj5

all: proj

proj: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf $(NAME).ps


zip:
	zip -r xlasmi00.zip Makefile $(NAME).tex obrazky


clean:
	rm -f *.nav *.snm *.toc
	rm -f obrazky/*.pdf
	rm -f *.dvi
	rm -f *.ps
	rm -f *.aux
	rm -f *.out
	rm -f *.log
	rm -f *.gz
	rm $(NAME).pdf *.zip
