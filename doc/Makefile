# $Id: Makefile,v 1.1 2004/05/23 09:25:02 qjhart Exp $

gs_TEXMFDIR = $(HOME)/texmf/tex/latex/geostreams

# If you add to this, you need to do mktexlsr yourself:)
gs_TEXMF:=geostreams.sty geostreams.bib

foo:
	@echo make install will add some texfiles into your personal texmf dir
	@echo dir=$(gs_TEXMFDIR)

install: $(patsubst %,$(gs_TEXMFDIR)/%,$(gs_TEXMF))

$(gs_TEXMFDIR)/%: %
	install -d $(gs_TEXMFDIR)
	install --mode=644 $< $(gs_TEXMFDIR)
