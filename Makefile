
HOST_CXX = g++

HOST_CXXFLAGS  = -g -O2 -Wall
HOST_CXXFLAGS += -Iinclude
HOST_CXXFLAGS += -lrt -lcurl


all: buildgear

%.o: %.cc
	$(HOST_CXX) $(HOST_CXXFLAGS) -c $< -o $@

buildgear: main.o \
	filesystem.o \
	buildfiles.o \
	time.o \
	dependency.o \
	source.o \
	options.o \
	download.o \
	configfile.o \
	tools.o
	$(HOST_CXX) $(HOST_CXXFLAGS) $^ -o $@
	strip $@

clean:
	rm -rf buildgear *.o
