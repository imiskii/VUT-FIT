#
# Projekt: proj3.tex
# Autor:   Michal Ľaš
# Datum:   27.03.2023
# 


NAME=proj3

all: proj

proj: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t -a4 $(NAME).dvi
	-sPAPERSIZE=a4 ps2pdf $(NAME).ps


over: all
	pdftk vzor.pdf multibackground $(NAME).pdf output overlay.pdf


zip:
	zip xlasmi00.zip Makefile $(NAME).tex *.eps


clean:
	rm -f *.dvi
	rm -f *.ps
	rm -f *.aux
	rm -f *.out
	rm -f *.log
	rm -f *.gz
	rm $(NAME).pdf
	rm etiopan*.pdf
	rm oniisan*.pdf
