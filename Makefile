CC=gcc
LATEXC=pdflatex
DOCC=doxygen
CFLAGS=-g -Wall 
CFLAGS2=-g -Wall -Wextra -std=c99

REFDIR=.
SRCDIR=$(REFDIR)/src
BINDIR=$(REFDIR)/bin
DOCDIR=$(REFDIR)/doc
REPORTDIR=$(REFDIR)/rapport

LATEXSOURCE=$(wildcard $(REPORTDIR)/*.tex)
CSOURCE=$(wildcard $(SRCDIR)/applyPatch.c) 
CSOURCE2=$(wildcard $(SRCDIR)/computePatchOpt.c)
PDF=$(LATEXSOURCE:.tex=.pdf)


all: binary report doc

$(BINDIR)/computePatchOpt: $(CSOURCE2)
	$(CC) $(CFLAGS2)  $^ -o $@

$(BINDIR)/applyPatch: $(CSOURCE)
	$(CC) $(CFLAGS)  $^ -o $@

%.pdf: $(LATEXSOURCE)
	$(LATEXC) -output-directory $(REPORTDIR) $^ 

$(DOCDIR)/index.html: $(SRCDIR)/Doxyfile $(CSOURCE) 
	$(DOCC) $(SRCDIR)/Doxyfile

patchOpt: $(BINDIR)/computePatchOpt

binary: $(BINDIR)/applyPatch  $(BINDIR)/computePatchOpt

report: $(PDF) 

doc: $(DOCDIR)/index.html

clean:
	rm -rf $(DOCDIR) $(BINDIR)/* $(REPORTDIR)/*.aux $(REPORTDIR)/*.log  $(REPORTDIR)/rapport.pdf 


.PHONY: all doc binary report 
