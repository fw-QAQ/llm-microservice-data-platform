CXX = g++

CXXFLAGS = -std=c++17 -O2 -pthread -I../../source

LDLIBS = -letcd-cpp-api -lcpprest -pthread

SCAFFOLD_SRC = ../../source/etcd.cc ../../source/util.cc

all: registry discovery

registry: registry.cc $(SCAFFOLD_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

discovery: discovery.cc $(SCAFFOLD_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f registry discovery