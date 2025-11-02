/*
TODO: Outputs one valid course-taking order using DFS-based topological sort.
@AUTHORS: Batuhan Sencer - Larry To
*/
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "graph.hpp"

int main() {
    // -----------------------------------------------------------------
    // TODO: List all courses (nodes). These come from the project handout.
    // @AUTHOR: Larry To
    // -----------------------------------------------------------------
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

    // Map each course string to an integer ID @AUTHOR: Batuhan Sencer
    std::unordered_map<std::string, int> id;
    for (int i = 0; i < (int)courses.size(); i++) {
        id[courses[i]] = i;
    }

    Graph g(courses.size());

    // -----------------------------------------------------------------
    // TODO: Add prerequisite edges prereq -> dependent
    //    (create the map from the assignment sheet)
    // @AUTHOR: Batuhan Sencer
    // -----------------------------------------------------------------
    auto prereq = [&](const std::string& pre, const std::string& post) {
        g.addEdge(id[pre], id[post]);
    };

    // CS 1411 -> CS 1412
    prereq("CS 1411 - Programming Principles I",
           "CS 1412 - Programming Principles II");

    // CS 1411 -> CS 1382
    prereq("CS 1411 - Programming Principles I",
           "CS 1382 - Discrete Computational Structures");

    // MATH 1451 -> MATH 1452
    prereq("MATH 1451 - Calculus I with Applications",
           "MATH 1452 - Calculus II with Applications");

    // MATH 1451 -> PHYS 1408
    prereq("MATH 1451 - Calculus I with Applications",
           "PHYS 1408 - Principles of Physics I");

    // ENGL 1301 -> ENGL 1302
    prereq("ENGL 1301 - Essentials of College Rhetoric",
           "ENGL 1302 - Advanced College Rhetoric");

    // CS 1412 -> CS 2413
    prereq("CS 1412 - Programming Principles II",
           "CS 2413 - Data Structures");

    // MATH 1452 -> MATH 2450
    prereq("MATH 1452 - Calculus II with Applications",
           "MATH 2450 - Calculus III with Applications");

    // PHYS 1408 -> PHYS 2401
    prereq("PHYS 1408 - Principles of Physics I",
           "PHYS 2401 - Principles of Physics II");

    // ENGL 1301 & ENGL 1302 -> ENGL 2311
    prereq("ENGL 1301 - Essentials of College Rhetoric",
           "ENGL 2311 - Introduction to Technical Writing");
    prereq("ENGL 1302 - Advanced College Rhetoric",
           "ENGL 2311 - Introduction to Technical Writing");

    // MATH 1451 -> ECE 2372
    prereq("MATH 1451 - Calculus I with Applications",
           "ECE 2372 - Modern Digital System Design");

    // CS 1412 & ECE 2372 -> CS 2350
    prereq("CS 1412 - Programming Principles II",
           "CS 2350 - Computer Organization and Assembly Language Programming");
    prereq("ECE 2372 - Modern Digital System Design",
           "CS 2350 - Computer Organization and Assembly Language Programming");

    // CS 2413 -> CS 2365
    prereq("CS 2413 - Data Structures",
           "CS 2365 - Object-Oriented Programming");

    // CS 2413 -> CS 3361
    prereq("CS 2413 - Data Structures",
           "CS 3361 - Concepts of Programming Languages");

    // CS 2413, CS 1382, MATH 2360 -> CS 3364
    prereq("CS 2413 - Data Structures",
           "CS 3364 - Design and Analysis of Algorithms");
    prereq("CS 1382 - Discrete Computational Structures",
           "CS 3364 - Design and Analysis of Algorithms");
    prereq("MATH 2360 - Linear Algebra",
           "CS 3364 - Design and Analysis of Algorithms");

    // MATH 2450 -> MATH 3342
    prereq("MATH 2450 - Calculus III with Applications",
           "MATH 3342 - Mathematical Statistics for Engineers and Scientists");

    // CS 2365, CS 2413, MATH 3342 -> CS 3365
    prereq("CS 2365 - Object-Oriented Programming",
           "CS 3365 - Software Engineering I");
    prereq("CS 2413 - Data Structures",
           "CS 3365 - Software Engineering I");
    prereq("MATH 3342 - Mathematical Statistics for Engineers and Scientists",
           "CS 3365 - Software Engineering I");

    // CS 2350 -> CS 3375
    prereq("CS 2350 - Computer Organization and Assembly Language Programming",
           "CS 3375 - Computer Architecture");

    // CS 1382 -> CS 3383
    prereq("CS 1382 - Discrete Computational Structures",
           "CS 3383 - Theory of Automata");

    // CS 3365 -> CS 4365
    prereq("CS 3365 - Software Engineering I",
           "CS 4365 - Software Engineering II");

    // CS 3364 & CS 3375 -> CS 4352
    prereq("CS 3364 - Design and Analysis of Algorithms",
           "CS 4352 - Operating Systems");
    prereq("CS 3375 - Computer Architecture",
           "CS 4352 - Operating Systems");

    // CS 3364 -> CS 4354
    prereq("CS 3364 - Design and Analysis of Algorithms",
           "CS 4354 - Concepts of Database Systems");

    // CS 4365 -> CS 4366
    prereq("CS 4365 - Software Engineering II",
           "CS 4366 - Senior Capstone Project");

    // -----------------------------------------------------------------
    // TODO: Topological sort
    // @AUTHOR: Batuhan Sencer
    // -----------------------------------------------------------------
    std::vector<int> order;
    try {
        order = g.topoSort();
    } catch (const std::exception& e) {
        std::cerr << "Error during topo sort: " << e.what() << "\n";
        return 1;
    }

    // -----------------------------------------------------------------
    // TODO: Print the result (one valid order)
    // @AUTHOR: Larry To
    // -----------------------------------------------------------------
    std::cout << "Valid course order:\n";
    for (int i = 0; i < (int)order.size(); i++) {
        int cid = order[i];
        std::cout << (i + 1) << ". " << courses[cid] << "\n";
    }

    return 0;
}
