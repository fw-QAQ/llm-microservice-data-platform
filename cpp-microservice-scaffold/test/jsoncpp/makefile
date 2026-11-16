CFLAGS=-I../../source/ -std=c++17

json_test: json_test.cc ../../source/json_util.cc
	g++ $(CFLAGS) $^ -o $@ -ljsoncpp -lgtest -pthread

clean:
	rm -f json_test