CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g

TARGET   = shortest_path
SRCS     = main.cpp            # <-- change this if your file name is different
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
