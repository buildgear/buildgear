bin_PROGRAMS = buildgear
buildgear_SOURCES = \
   signals.cc \
   fakeroot.cc \
   buildfile.cc \
   buildfiles.cc \
   buildmanager.cc \
   buildsystem.cc \
   clock.cc \
   utility.cc \
   config.cc \
   configfile.cc \
   dependency.cc \
   download.cc \
   filesystem.cc \
   main.cc \
   options.cc \
   source.cc \
   svg.cc \
   cursor.cc \
   log.cc \
   include/buildgear/signals.h \
   include/buildgear/fakeroot.h \
   include/buildgear/buildfile.h \
   include/buildgear/buildfiles.h \
   include/buildgear/buildmanager.h \
   include/buildgear/buildsystem.h \
   include/buildgear/clock.h \
   include/buildgear/utility.h \
   include/buildgear/configfile.h \
   include/buildgear/config.h \
   include/buildgear/debug.h \
   include/buildgear/dependency.h \
   include/buildgear/download.h \
   include/buildgear/filesystem.h \
   include/buildgear/options.h \
   include/buildgear/source.h \
   include/buildgear/svg.h \
   include/buildgear/cursor.h \
   include/buildgear/log.h
buildgear_CPPFLAGS = -I$(srcdir)/include
dist_pkgdata_SCRIPTS = config.guess \
                       buildgear.sh
dist_pkgdata_DATA = template.config \
                    template.readme \
                    template.buildgearconfig
AM_CXXFLAGS = -DAM_PKGDATADIR='"$(pkgdatadir)"'
bashcompletiondir=$(sysconfdir)/bash_completion.d
dist_bashcompletion_DATA=buildgear-bash-completion.sh
