# Builds:
#   - topo_courses     (DFS topological order)
#   - plan_semesters   (layered semester plan)
# @AUTHOR: Batuhan Sencer

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

# Executables we want
TARGETS = topo_courses plan_semesters

all: $(TARGETS)

# Build topo_courses (uses graph.hpp)
topo_courses: src/topo_courses.cpp include/graph.hpp
	$(CXX) $(CXXFLAGS) -o topo_courses src/topo_courses.cpp

# Build plan_semesters (standalone)
plan_semesters: src/plan_semesters.cpp
	$(CXX) $(CXXFLAGS) -o plan_semesters src/plan_semesters.cpp

# Run helpers
run_topo: topo_courses
	./topo_courses

run_semesters: plan_semesters
	./plan_semesters

clean:
	rm -f $(TARGETS)

rebuild: clean all
