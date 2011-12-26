
CXX = g++

CXXFLAGS  = -Wall -O2
CXXFLAGS += -Iinclude -Ilib/lemon-1.2.1
CXXFLAGS += -L.
CXXFLAGS += -lrt -lcurl-gnutls -lemon


all: libemon.a buildgear

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

libemon.a:
	tar -C lib -xf lib/lemon-nodoc-1.2.1.tar.gz
	(cd lib/lemon-1.2.1 ; \
	./configure --without-glpk \
					--without-cplex \
					--without-soplex \
					--without-coin \
					--disable-tools \
					--disable-dependency-tracking \
					--enable-static ; \
   make ; \
	cp lemon/.libs/libemon.a ../.. )

buildgear: main.o \
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
	buildsystem.o \
	config.o \
   thread.o \
   libemon.a
	$(CXX) $^ -o $@ $(CXXFLAGS)
	strip $@

clean:
	rm -rf buildgear *.o
	rm -rf lib/lemon-1.2.1 libemon.a
