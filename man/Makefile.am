SUFFIXES = .man1 .1 .man5 .5

.man1.1:
	sed -f author.sed \
            -e "s/<===VERSION===>/$(VERSION)/g" \
            -e "s/<===DATE===>/`date '+%d %b %Y'`/g" \
            -e "/<===BUILDGEAR_CONFIG_TEMPLATE===>/r \
                ${abs_top_srcdir}/src/template.buildgearconfig" \
            -e "/<===BUILDGEAR_CONFIG_TEMPLATE===>/d" \
            -e "/<===BUILDFILE_CONFIG_TEMPLATE===>/r \
                ${abs_top_srcdir}/src/template.config" \
            -e "/<===BUILDFILE_CONFIG_TEMPLATE===>/d" \
            $< > $@

.man5.5:
	sed -f author.sed \
            -e "s/<===VERSION===>/$(VERSION)/g" \
            -e "s/<===DATE===>/`date '+%d %b %Y'`/g" \
            -e "/<===BUILDGEAR_CONFIG_TEMPLATE===>/r \
                ${abs_top_srcdir}/src/template.buildgearconfig" \
            -e "/<===BUILDGEAR_CONFIG_TEMPLATE===>/d" \
            -e "/<===BUILDFILE_CONFIG_TEMPLATE===>/r \
                ${abs_top_srcdir}/src/template.config" \
            -e "/<===BUILDFILE_CONFIG_TEMPLATE===>/d" \
            $< > $@

man1_sources = buildgear.man1 \
               buildgear-clean.man1 \
               buildgear-download.man1 \
               buildgear-show.man1 \
               buildgear-build.man1 \
               buildgear-init.man1 \
               buildgear-config.man1

man5_sources = buildgear-config.man5 \
               buildgear-buildfile.man5 \
               buildgear-buildfileconfig.man5

dist_man1_MANS = $(man1_sources:.man1=.1)
dist_man5_MANS = $(man5_sources:.man5=.5)

MAINTAINERCLEANFILES = $(dist_man1_MANS) $(dist_man5_MANS)
