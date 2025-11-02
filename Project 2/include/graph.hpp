/*
Simple directed graph with DFS-based topological sort.
Used for course prerequisite ordering in CS 3364 Project 2.
@Authors: Batuhan Sencer - Larry To
*/
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <stack>
#include <stdexcept>

class Graph {
public:
    // TODO: Need to create a graph with n nodes (0..n-1)
    Graph(int n)
        : adj(n), visited(n, 0) {}

    // Add edge u -> v meaning:
    // u must come before v
    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    // Perform DFS-based Topological Sort.
    // Returns a vector<int> of node IDs in valid order.
    // Throws runtime_error if cycle is detected.
    std::vector<int> topoSort() {
        std::stack<int> st;
        for (int node = 0; node < (int)adj.size(); node++) {
            if (visited[node] == 0) {
                dfs(node, st);
            }
        }

        std::vector<int> result;
        while (!st.empty()) {
            result.push_back(st.top());
            st.pop();
        }
        return result;
    }

private:
    std::vector<std::vector<int>> adj;
    // 0 = not visited, 1 = visiting, 2 = finished
    std::vector<int> visited;

    void dfs(int u, std::stack<int>& st) {
        if (visited[u] == 1) {
            // hits a gray node => back edge => cycle.
            throw std::runtime_error("Cycle detected in prerequisites!");
        }
        if (visited[u] == 2) {
            return; // already processed
        }

        visited[u] = 1; // mark as visiting

        for (int v : adj[u]) {
            dfs(v, st);
        }

        visited[u] = 2; // done
        st.push(u);
    }
};

#endif
