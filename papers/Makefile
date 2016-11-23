PAPER = sig-alternate-sample
TEX = $(wildcard *.tex)
BIB = sigproc.bib
FIGS = $(wildcard figures/*.eps figures/*.pdf figures/*.png)

.PHONY: all clean

$(PAPER).pdf: $(TEX) $(BIB) $(FIGS) sig-alternate-05-2015.cls
	echo $(FIGS)
	pdflatex --shell-escape $(PAPER)
	bibtex $(PAPER)
	pdflatex --shell-escape $(PAPER)
	pdflatex --shell-escape $(PAPER)

clean:
	rm -f *.aux *.bbl *.blg *.log *.out $(PAPER).pdf

