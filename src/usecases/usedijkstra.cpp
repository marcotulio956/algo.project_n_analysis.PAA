#include "usecases/graphs/usedijkstra.hpp"
#include <iostream>
#include <string>
#include <iomanip>

#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

using namespace std;
using namespace graph_algo;

void use_disjktra() {
    // ########## use case 4
    Graph<string, int, EdgeInfo> g4(false);

    // 1) Add 15 nodes with id 1..15 and a string payload
    for (int id = 1; id <= 15; ++id) {
        g4.add_node(id, string("Node") + to_string(id));
    }

    // 2) Define edges (from, to, EdgeInfo)
    vector<tuple<int,int,EdgeInfo>> edges = {
        {1, 2,  {"a-1-2", 1.2, 10, true}},
        {1, 3,  {"a-1-3", 2.5,  8, true}},
        {2, 4,  {"a-2-4", 1.7, 12, true}},
        {2, 5,  {"a-2-5", 2.0,  6, false}},
        {3, 6,  {"a-3-6", 0.9, 14, true}},
        {3, 7,  {"a-3-7", 3.1,  4, true}},
        {4, 8,  {"a-4-8", 2.2,  9, true}},
        {5, 9,  {"a-5-9", 1.1, 11, true}},
        {6,10,  {"a-6-10",2.6,  7, true}},
        {7,11,  {"a-7-11",1.9, 13, false}},
        {8,12,  {"a-8-12",2.4,  5, true}},
        {9,13,  {"a-9-13",0.5, 20, true}},
        {10,14, {"a-10-14",3.0, 3, true}},
        {11,15, {"a-11-15",2.8, 2, true}},
        {4, 5,  {"a-4-5", 1.3,  8, true}},
        {5, 6,  {"a-5-6", 0.7, 10, true}},
        {7, 9,  {"a-7-9", 2.2,  6, true}},
        {12,13, {"a-12-13",1.0, 4, true}},
        {13,14, {"a-13-14",1.4, 7, true}},
        {14,15, {"a-14-15",0.6, 9, true}},
        {1,15,  {"a-1-15",5.0, 1, false}}
    };

    // 3) Add edges to graph
    for (const auto &t : edges) {
        int u = get<0>(t);
        int v = get<1>(t);
        EdgeInfo info = get<2>(t);
        g4.add_edge(u, v, info);
    }

    // 4) Print basic info
    cout << "Graph: nodes=" << g4.node_count()
              << ", edges=" << g4.edge_count() << "\n\n";

    for (auto &[id, name] : g4.list_nodes())
        cout << "Node " << id << ": " << name << "\n";

    // List all edges
    for (auto &[u, v, prop] : g4.list_edges())
        cout << u << " -> " << v << " : " << prop << "\n";

    cout << "\n";

    // 5) BFS from node 1 (now as free function)
    auto bfs_order = graph_algo::bfs(g4, 1);
    cout << "BFS order from 1: ";
    for (int id : bfs_order) cout << id << " ";
    cout << "\n\n";

    // 6) Dijkstra using extractor that reads weight from EdgeInfo (free function)
    auto extractor2 = [](const EdgeInfo &e)->double {
        // Optionally ignore inactive edges by returning a huge weight
        if (!e.active) return numeric_limits<double>::infinity();
        return e.weight;
    };

    auto [distances, prev] = graph_algo::dijkstra(g4, 1, extractor2);

    // Print distances
    cout << fixed << setprecision(2);
    cout << "Distances from 1 (using EdgeInfo.weight, inactive edges = unreachable):\n";
    for (int id = 1; id <= 15; ++id) {
        double d = distances[id];
        if (d == numeric_limits<double>::infinity()) {
            cout << id << ": unreachable\n";
        } else {
            cout << id << ": " << d << "\n";
        }
    }

    // Reconstruct and print path to a node (example: to node 14)
    int target = 14;
    // specify graph type so reconstruct_path can deduce id_type
    using MyGraph = Graph<string, int, EdgeInfo>;
    auto path = graph_algo::reconstruct_path<MyGraph>(prev, target);

    if (path.empty() || distances[target] == numeric_limits<double>::infinity()) {
        cout << "\nNo path (reachable) found from 1 to " << target << "\n";
    } else {
        cout << "\nPath 1 -> " << target << " (by ids): ";
        for (auto id : path) cout << id << " ";
        cout << "\nDistance: " << distances[target] << "\n";
    }

    cout << "\nMermaid syntax:\n" << g4.to_mermaid() << "\n";
}