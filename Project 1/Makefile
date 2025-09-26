CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 

all: rank_reliability

rank_reliability: rank_reliability.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	./rank_reliability --out out_rank source1.txt source2.txt source3.txt source4.txt source5.txt 


clean:
	rm -f rank_reliability
