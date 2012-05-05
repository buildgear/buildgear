
CXX = g++

CXXFLAGS  = -Wall -O2
CXXFLAGS += -Iinclude
CXXFLAGS += -L.
CXXFLAGS += -lrt -lcurl-gnutls


all: buildgear

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
	svg.o
	$(CXX) $^ -o $@ $(CXXFLAGS)
	strip $@

clean:
	rm -rf buildgear *.o
