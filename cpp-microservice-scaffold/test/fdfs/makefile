CXX = g++

CXXFLAGS = -std=c++17 -O2 -pthread -I../../source

LDLIBS = -lfdfsclient -lfastcommon -pthread

SCAFFOLD_SRC = ../../source/fdfs.cc

all: test

test: test.cc $(SCAFFOLD_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f test