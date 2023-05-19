#
# Projekt: proj2.tex
# Autor:   Michal Ľaš
# Datum:   05.03.2023
# 


NAME=proj2

all: proj

proj: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t -a4 $(NAME).dvi
	ps2pdf $(NAME).ps


over: all
	pdftk vzor.pdf multibackground $(NAME).pdf output overlay.pdf


clean:
	rm -f *.dvi
	rm -f *.ps
	rm -f *.aux
	rm -f *.out
	rm -f *log
	rm $(NAME).pdf
