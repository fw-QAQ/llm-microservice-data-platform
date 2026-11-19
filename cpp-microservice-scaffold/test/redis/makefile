CXX = g++

CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pthread -I../../source

LDLIBS = -lredis++ -lhiredis -pthread

TARGET = test

SRCS = test.cc ../../source/redis.cc

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET)