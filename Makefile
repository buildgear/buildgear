
BUILD_CXX = g++

BUILD_CXXFLAGS  = -Wall -O2
BUILD_CXXFLAGS += -Iinclude -Ilib/lemon-1.2.1/lemon
BUILD_CXXFLAGS += -L.
BUILD_CXXFLAGS += -lrt -lcurl-gnutls -lemon


all: buildgear

%.o: %.cc
	$(BUILD_CXX) $(BUILD_CXXFLAGS) -c $< -o $@

libemon.a:
	tar -C lib -xf lib/lemon-nodoc-1.2.1.tar.gz
	(cd lib/lemon-1.2.1 ; \
	./configure --without-glpk --without-cplex --without-soplex --without-coin --disable-tools --disable-dependency-tracking --enable-static ; \
   make ; \
	cp lemon/.libs/libemon.a ../.. )

buildgear: libemon.a \
	main.o \
	filesystem.o \
	buildfile.o \
	buildfiles.o \
	clock.o \
	dependency.o \
	source.o \
	options.o \
	download.o \
	configfile.o \
	buildmanager.o \
	tools.o \
	config.o
	$(BUILD_CXX) $(BUILD_CXXFLAGS) $^ -o $@
	strip $@

clean:
	rm -rf buildgear *.o
	rm -rf lib/lemon-1.2.1 libemon.a
