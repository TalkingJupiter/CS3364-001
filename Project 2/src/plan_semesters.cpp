// TODO: Prints a semester-by-semester plan using layered topological order.
// @AUTHORS: Batuhan Sencer - Larry To

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>

int main() {
    // ------------------------------------------------------------
    // TODO: List all courses (nodes)
    // @AUTHOR: Larry To
    // ------------------------------------------------------------
    std::vector<std::string> courses = {
        "CS 1411 - Programming Principles I",
        "MATH 1451 - Calculus I with Applications",
        "ENGL 1301 - Essentials of College Rhetoric",
        "CS 1412 - Programming Principles II",
        "MATH 1452 - Calculus II with Applications",
        "PHYS 1408 - Principles of Physics I",
        "ENGL 1302 - Advanced College Rhetoric",
        "CS 2413 - Data Structures",
        "CS 1382 - Discrete Computational Structures",
        "ECE 2372 - Modern Digital System Design",
        "MATH 2450 - Calculus III with Applications",
        "PHYS 2401 - Principles of Physics II",
        "CS 2350 - Computer Organization and Assembly Language Programming",
        "CS 2365 - Object-Oriented Programming",
        "ENGR 2392 - Engineering Ethics and Its Impact on Society",
        "POLS 1301 - American Government",
        "MATH 2360 - Linear Algebra",
        "ENGL 2311 - Introduction to Technical Writing",
        "CS 3361 - Concepts of Programming Languages",
        "CS 3364 - Design and Analysis of Algorithms",
        "MATH 3342 - Mathematical Statistics for Engineers and Scientists",
        "POLS 2306 - Texas Politics and Topics",
        "CS 3365 - Software Engineering I",
        "CS 3375 - Computer Architecture",
        "CS 3383 - Theory of Automata",
        "CS 4365 - Software Engineering II",
        "CS 4352 - Operating Systems",
        "CS 4354 - Concepts of Database Systems",
        "CS 4366 - Senior Capstone Project"
    };

    // map course name -> id
    std::unordered_map<std::string,int> id;
    for (int i = 0; i < (int)courses.size(); i++) {
        id[courses[i]] = i;
    }

    int n = courses.size();

    // ------------------------------------------------------------
    // TODO: Build adjacency and in-degrees
    // @AUTHOR: Batuhan Sencer - Larry To
    // ------------------------------------------------------------
    std::vector<std::vector<int>> adj(n);
    std::vector<int> inDegree(n, 0);

    auto addEdge = [&](const std::string& pre, const std::string& post) {
        int u = id[pre];
        int v = id[post];
        adj[u].push_back(v);
        inDegree[v]++;
    };

    // Same prereq list as topo_courses.cpp
    addEdge("CS 1411 - Programming Principles I",
            "CS 1412 - Programming Principles II");
    addEdge("CS 1411 - Programming Principles I",
            "CS 1382 - Discrete Computational Structures");

    addEdge("MATH 1451 - Calculus I with Applications",
            "MATH 1452 - Calculus II with Applications");
    addEdge("MATH 1451 - Calculus I with Applications",
            "PHYS 1408 - Principles of Physics I");

    addEdge("ENGL 1301 - Essentials of College Rhetoric",
            "ENGL 1302 - Advanced College Rhetoric");

    addEdge("CS 1412 - Programming Principles II",
            "CS 2413 - Data Structures");

    addEdge("MATH 1452 - Calculus II with Applications",
            "MATH 2450 - Calculus III with Applications");

    addEdge("PHYS 1408 - Principles of Physics I",
            "PHYS 2401 - Principles of Physics II");

    addEdge("ENGL 1301 - Essentials of College Rhetoric",
            "ENGL 2311 - Introduction to Technical Writing");
    addEdge("ENGL 1302 - Advanced College Rhetoric",
            "ENGL 2311 - Introduction to Technical Writing");

    addEdge("MATH 1451 - Calculus I with Applications",
            "ECE 2372 - Modern Digital System Design");

    addEdge("CS 1412 - Programming Principles II",
            "CS 2350 - Computer Organization and Assembly Language Programming");
    addEdge("ECE 2372 - Modern Digital System Design",
            "CS 2350 - Computer Organization and Assembly Language Programming");

    addEdge("CS 2413 - Data Structures",
            "CS 2365 - Object-Oriented Programming");

    addEdge("CS 2413 - Data Structures",
            "CS 3361 - Concepts of Programming Languages");

    addEdge("CS 2413 - Data Structures",
            "CS 3364 - Design and Analysis of Algorithms");
    addEdge("CS 1382 - Discrete Computational Structures",
            "CS 3364 - Design and Analysis of Algorithms");
    addEdge("MATH 2360 - Linear Algebra",
            "CS 3364 - Design and Analysis of Algorithms");

    addEdge("MATH 2450 - Calculus III with Applications",
            "MATH 3342 - Mathematical Statistics for Engineers and Scientists");

    addEdge("CS 2365 - Object-Oriented Programming",
            "CS 3365 - Software Engineering I");
    addEdge("CS 2413 - Data Structures",
            "CS 3365 - Software Engineering I");
    addEdge("MATH 3342 - Mathematical Statistics for Engineers and Scientists",
            "CS 3365 - Software Engineering I");

    addEdge("CS 2350 - Computer Organization and Assembly Language Programming",
            "CS 3375 - Computer Architecture");

    addEdge("CS 1382 - Discrete Computational Structures",
            "CS 3383 - Theory of Automata");

    addEdge("CS 3365 - Software Engineering I",
            "CS 4365 - Software Engineering II");

    addEdge("CS 3364 - Design and Analysis of Algorithms",
            "CS 4352 - Operating Systems");
    addEdge("CS 3375 - Computer Architecture",
            "CS 4352 - Operating Systems");

    addEdge("CS 3364 - Design and Analysis of Algorithms",
            "CS 4354 - Concepts of Database Systems");

    addEdge("CS 4365 - Software Engineering II",
            "CS 4366 - Senior Capstone Project");

    // ------------------------------------------------------------
    // TODOs: Do semester layering using Kahn-style iteration
    //    - Find all nodes with inDegree == 0 -> Semester 1
    //    - Remove them, decrease inDegree of neighbors
    //    - New zero inDegree nodes -> Semester 2, etc.
    // @AUTHOR: Batuhan Sencer
    // ------------------------------------------------------------
    std::vector<std::vector<int>> semesters;
    std::vector<int> inDeg = inDegree; // copy
    std::set<int> available;

    for (int i = 0; i < n; i++) {
        if (inDeg[i] == 0) {
            available.insert(i);
        }
    }

    int placedCount = 0;
    while (!available.empty()) {
        // current semester is everything that is now available
        std::vector<int> thisSem(available.begin(), available.end());
        semesters.push_back(thisSem);

        // prepare for next semester
        std::set<int> nextAvailable;

        for (int u : thisSem) {
            placedCount++;
            // remove outgoing edges u -> v
            for (int v : adj[u]) {
                inDeg[v]--;
                if (inDeg[v] == 0) {
                    nextAvailable.insert(v);
                }
            }
        }

        available = nextAvailable;
    }

    if (placedCount != n) {
        std::cerr << "ERROR: Cycle detected. Cannot build semester plan.\n";
        return 1;
    }

    // ------------------------------------------------------------
    // TODO: Print semester plan
    // @AUTHOR: Larry To
    // ------------------------------------------------------------
    for (int s = 0; s < (int)semesters.size(); s++) {
        std::cout << "Semester " << (s + 1) << ":\n";
        for (int cid : semesters[s]) {
            std::cout << "  - " << courses[cid] << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "Total semesters (dependency layers): "
              << semesters.size() << "\n";

    return 0;
}
