CXX = g++
CXXFLAGS = -Wall -std=c++17

SRCS = main.cpp benchmark.cpp allocator.cpp free_list_helpers.cpp debug.cpp
TARGET = allocator

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
