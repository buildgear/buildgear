#
# Makefile template for generating the Build Gear Handbook in html and pdf.
#

XSLTPROC = xsltproc
FOP = fop
HTML_XSL = html.xsl
PDF_XSL = pdf.xsl
DOCBOOKS = handbook.docbook \
           introduction.docbook \
           installation.docbook \
           basics.docbook \
           usage.docbook \
           faq.docbook \
           appendix.docbook \
           example.docbook

html-local: $(PACKAGE_TARNAME)-v$(PACKAGE_VERSION)-handbook.html
pdf-local: $(PACKAGE_TARNAME)-v$(PACKAGE_VERSION)-handbook.pdf

.doc.docbook:
	sed 's/PACKAGE_VERSION/$(PACKAGE_VERSION)/g' $< > $@

$(PACKAGE_TARNAME)-v$(PACKAGE_VERSION)-handbook.html: $(DOCBOOKS) $(HTML_XSL)
	$(XSLTPROC) -o $@ $(HTML_XSL) $<

$(PACKAGE_TARNAME)-v$(PACKAGE_VERSION)-handbook.pdf: $(DOCBOOKS) $(PDF_XSL)
	$(XSLTPROC) -o handbook.fo $(PDF_XSL) $<
	$(FOP) handbook.fo -pdf $@

clean-local:
	rm -f *.{fo,docbook}
	rm -f *.{pdf,html}
