dist_man_MANS = buildgear.1 buildgear-clean.1 buildgear.config.7 \
                buildgear-download.1 buildgear-show.1
SUFFIXES = .in
buildgear.1: buildgear.1.in
	sed -f version.sed -f date.sed -f author.sed $< > $@
buildgear-clean.1: buildgear-clean.1.in
	sed -f version.sed -f date.sed -f author.sed $< > $@
buildgear.config.7: buildgear.config.7.in
	sed -f version.sed -f date.sed -f author.sed $< > $@
buildgear-download.1: buildgear-download.1.in
	sed -f version.sed -f date.sed -f author.sed $< > $@
buildgear-show.1: buildgear-show.1.in
	sed -f version.sed -f date.sed -f author.sed $< > $@

clean-local:
	rm -f $(dist_man_MANS)
