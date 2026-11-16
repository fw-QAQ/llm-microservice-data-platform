CXX = g++

CXXFLAGS = -std=c++17 -O2 -pthread -I../../source

LDLIBS = -lbrpc -lleveldb -lgflags -lssl -lcrypto -lprotobuf -lz -ldl -lgtest -pthread

all: channels_test

channels_test: channels_test.cc ../../source/rpc.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f channels_test