#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <string>
#include <utility>

using namespace std;

const long long INF = 1e18;

// ------------------------------------------------------------------
// Helper: print path from source to v using parent[] array
// ------------------------------------------------------------------
void printPath(int v, const vector<int>& parent) {
    vector<int> path;
    for (int cur = v; cur != -1; cur = parent[cur]) {
        path.push_back(cur);
    }
    reverse(path.begin(), path.end());
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " -> ";
    }
}

// ------------------------------------------------------------------
// Dijkstra (Greedy)
// ------------------------------------------------------------------
void dijkstra(int n, int src,
              const vector<vector<pair<int,int>>>& adj,
              vector<long long>& dist,
              vector<int>& parent)
{
    dist.assign(n, INF);
    parent.assign(n, -1);

    using P = pair<long long,int>; // {distance, node}
    priority_queue<P, vector<P>, greater<P>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d != dist[u]) continue; // outdated entry

        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
}

// ------------------------------------------------------------------
// Bellman-Ford (Dynamic Programming)
// ------------------------------------------------------------------
struct Edge {
    int u, v, w;
};

bool bellmanFord(int n, int src,
                 const vector<Edge>& edges,
                 vector<long long>& dist,
                 vector<int>& parent)
{
    dist.assign(n, INF);
    parent.assign(n, -1);
    dist[src] = 0;

    // Relax edges n-1 times
    for (int i = 0; i < n - 1; ++i) {
        bool changed = false;
        for (const auto& e : edges) {
            if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w;
                parent[e.v] = e.u;
                changed = true;
            }
            if (dist[e.v] != INF && dist[e.v] + e.w < dist[e.u]) {
                dist[e.u] = dist[e.v] + e.w;
                parent[e.u] = e.v;
                changed = true;
            }
        }
        if (!changed) break;
    }

    // Check for negative cycles (should not exist here)
    for (const auto& e : edges) {
        if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) {
            return false; // negative cycle
        }
        if (dist[e.v] != INF && dist[e.v] + e.w < dist[e.u]) {
            return false;
        }
    }
    return true;
}

int main() {
    // number of buildings (vertices)
    const int N = 19;
    const int CS = 2; // index for "Computer Science"

    // For readability you can also store names in an array.
    vector<string> names = {
        "College Square",
        "Lewis Science Center",
        "Computer Science",
        "Prince Center",
        "Torreyson Library",
        "Old Main",
        "Fine Art",
        "Police Dept.",
        "Student Health Center",
        "Student Center",
        "McAlister Hall",
        "Wingo",
        "New Business Building",
        "Brewer-Hegeman",
        "Bear Village Apt.",
        "Burdick",
        "Speech Language Hearing",
        "Maintenance College",
        "Oak Tree Apt."
    };

    vector<vector<pair<int,int>>> adj(N);
    vector<Edge> edges;

    auto addEdge = [&](int a, int b, int w) {
        adj[a].push_back({b, w});
        adj[b].push_back({a, w}); // undirected
        edges.push_back({a, b, w});
    };

    // ----- add all edges from the map -----
    addEdge(0, 1, 200);
    addEdge(0, 3, 300);

    addEdge(1, 2, 150);
    addEdge(1, 16, 250);

    addEdge(2, 3, 80);
    addEdge(2, 4, 40);
    addEdge(2, 15, 30);

    addEdge(3, 4, 30);
    addEdge(3, 7, 100);

    addEdge(4, 5, 30);
    addEdge(4, 15, 80);

    addEdge(5, 6, 90);
    addEdge(5, 7, 200);
    addEdge(5, 10, 100);

    addEdge(6, 7, 50);
    addEdge(6, 9, 80);
    addEdge(6, 10, 180);

    addEdge(7, 8, 100);

    addEdge(8, 9, 50);
    addEdge(8, 12, 200);

    addEdge(9, 10, 100);
    addEdge(9, 12, 110);

    addEdge(10, 15, 200);
    addEdge(10, 11, 50);
    addEdge(10, 17, 150);

    addEdge(11, 12, 50);
    addEdge(11, 17, 100);

    addEdge(12, 13, 20);
    addEdge(12, 18, 30);

    addEdge(13, 14, 350);

    addEdge(15, 16, 100);
    addEdge(15, 17, 300);

    addEdge(16, 17, 120);

    addEdge(17, 12, 150);
    addEdge(17, 18, 160);

    addEdge(18, 13, 40);

    // -------------------- Dijkstra --------------------
    vector<long long> distD;
    vector<int> parentD;
    dijkstra(N, CS, adj, distD, parentD);

    cout << "=== Dijkstra (from Computer Science) ===\n";
    for (int v = 0; v < N; ++v) {
        cout << v << " : " << names[v] << "\n";
        if (distD[v] == INF) {
            cout << "   unreachable\n";
        } else {
            cout << "   distance = " << distD[v] << "\n";
            cout << "   path     = ";
            printPath(v, parentD);
            cout << "\n";
        }
        cout << "\n";
    }

    // -------------------- Bellman-Ford --------------------
    vector<long long> distB;
    vector<int> parentB;
    bool ok = bellmanFord(N, CS, edges, distB, parentB);

    if (!ok) {
        cout << "Graph has a negative cycle (should NOT happen here).\n";
        return 0;
    }

    cout << "=== Bellman-Ford (from Computer Science) ===\n";
    for (int v = 0; v < N; ++v) {
        cout << v << " : " << names[v] << "\n";
        if (distB[v] == INF) {
            cout << "   unreachable\n";
        } else {
            cout << "   distance = " << distB[v] << "\n";
            cout << "   path     = ";
            printPath(v, parentB);
            cout << "\n";
        }
        cout << "\n";
    }

    return 0;
}
