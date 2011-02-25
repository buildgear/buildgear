
HOST_CXX = g++

HOST_CXXFLAGS  = -Wall -O2
HOST_CXXFLAGS += -Iinclude
HOST_CXXFLAGS += -lrt -lcurl


all: buildgear

%.o: %.cc
	$(HOST_CXX) $(HOST_CXXFLAGS) -c $< -o $@

buildgear: main.o \
	filesystem.o \
	buildfiles.o \
	clock.o \
	dependency.o \
	source.o \
	options.o \
	download.o \
	configfile.o \
	tools.o \
	config.o
	$(HOST_CXX) $(HOST_CXXFLAGS) $^ -o $@
	strip $@

clean:
	rm -rf buildgear *.o
