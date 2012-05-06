bin_PROGRAMS = buildgear
buildgear_SOURCES = \
buildfile.cc \
buildfiles.cc \
buildmanager.cc \
buildsystem.cc \
clock.cc \
config.cc \
configfile.cc \
dependency.cc \
download.cc \
filesystem.cc \
main.cc \
options.cc \
source.cc \
svg.cc \
thread.cc \
include/buildgear/buildfile.h \
include/buildgear/buildfiles.h \
include/buildgear/buildmanager.h \
include/buildgear/buildsystem.h \
include/buildgear/clock.h \
include/buildgear/configfile.h \
include/buildgear/config.h \
include/buildgear/debug.h \
include/buildgear/dependency.h \
include/buildgear/download.h \
include/buildgear/filesystem.h \
include/buildgear/options.h \
include/buildgear/source.h \
include/buildgear/svg.h \
include/buildgear/thread.h
buildgear_CPPFLAGS = -I$(srcdir)/include
dist_pkgdata_SCRIPTS = config.guess buildgear.sh
AM_CXXFLAGS = -DAM_PKGDATADIR='"$(pkgdatadir)"'
