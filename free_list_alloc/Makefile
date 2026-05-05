CXX = g++
CXXFLAGS = -Wall -std=c++17

SRCS = main.cpp allocator.cpp split.cpp debug.cpp
TARGET = allocator

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
