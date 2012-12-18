SUFFIXES = .man1 .1 .man7 .7

.man1.1:
	sed -f author.sed \
            -e "s/<===VERSION===>/$(VERSION)/g" \
            -e "s/<===DATE===>/`date '+%d %b %Y'`/g" \
            $< > $@

.man7.7:
	sed -f author.sed \
            -e "s/<===VERSION===>/$(VERSION)/g" \
            -e "s/<===DATE===>/`date '+%d %b %Y'`/g" \
            $< > $@

man1_sources = buildgear.man1 \
               buildgear-clean.man1 \
               buildgear-download.man1 \
               buildgear-show.man1 \
               buildgear-build.man1

man7_sources = buildgear.config.man7

dist_man1_MANS = $(man1_sources:.man1=.1)
dist_man7_MANS = $(man7_sources:.man7=.7)

CLEANFILES = $(dist_man1_MANS) $(dist_man7_MANS)
