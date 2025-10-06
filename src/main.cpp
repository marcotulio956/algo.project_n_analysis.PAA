// main.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <memory>
#include <tuple>
#include <string>

#include "sortings.h"
#include "graph.h"
#include "graph_algorithms.hpp"   // fixed include
#include "node.h"
#include "util.h"

// ----------------- helper types and operator<< -----------------
struct EdgeInfo {
    std::string label; // name/label of the edge
    double weight;     // numeric weight (for shortest-path)
    int capacity;      // arbitrary integer property
    bool active;       // boolean flag
};

// helper to print edge info
std::ostream& operator<<(std::ostream& os, const EdgeInfo& e) {
    os << "{label:" << e.label
       << ", weight:" << e.weight
       << ", cap:" << e.capacity
       << ", active:" << (e.active ? "Y" : "N") << "}";
    return os;
}

// ----------------- test functions -----------------
void test_sortings() {
    std::vector<int> arr = {64, 25, 12, 22, 11, 07, 90, 45, 23, 67, 89};
    std::cout << "original array: "; Util::print_vector(arr);
    Sortings::insertion_sort(arr);

    arr = {13, 46, 24, 52, 20, 9, 78, 34, 56, 12, 01};
    std::cout << "original array: "; Util::print_vector(arr);
    Sortings::selection_sort(arr, false);

    arr = {23, 45, 12, 67, 34, 89, 90, 11, 78, 56, 01};
    std::cout << "original array: "; Util::print_vector(arr);
    Sortings::merge_sort(arr);

    arr = {1, 2, 3};
    std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
    std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;

    arr = {3 , 2, 1};
    std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
    std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;

    arr = {3};
    std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
    std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;

    arr = {3, 3, 3};
    std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
    std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;

    arr = {3, 3, 2, 1, 1, 2};
    for (size_t i=0; i<arr.size(); i++) {
        std::cout<< "nth greatest: " << i+1 << " is " << Sortings::find_nthgreatest(arr, i+1) << std::endl;
    }
}

void test_graph_usecase1(){
    // ########## use case 1
    struct W { double w; }; // edge property with a numeric field

    Graph<std::string, int, W> g1(true);
    g1.add_node(1, "A");
    g1.add_node(2, "B");
    g1.add_edge(1, 2, W{2.5});

    auto extractor1 = [](const W& p)->double { return p.w; }; // extractor for edge weight

    // Use free function from graph_algorithms.hpp
    auto [dist1, prev1] = graph_algo::dijkstra(g1, 1, extractor1);

    // print distances (example)
    std::cout << "Distances from 1 in g1:\n";
    for (const auto &p : g1.list_nodes()) {
        int id = p.first;
        double d = dist1[id];
        if (d == std::numeric_limits<double>::infinity()) std::cout << id << ": unreachable\n";
        else std::cout << id << ": " << d << "\n";
    }
    std::cout << "\n";
}

void test_graph_usecase2(){
    // ########## use case 2
    struct EdgeInfo2 { std::string name; double cost; int capacity; };

    Graph<int, int, EdgeInfo2> g2(false);
    g2.add_edge(1, 2, {"road-12", 3.5, 100});

    auto get_cost = [](const EdgeInfo2& e){ return e.cost; };
    auto [dist2, prev2] = graph_algo::dijkstra(g2, 1, get_cost);

    // Graph with only label as edges with no numerical weight
    Graph<int, int, std::string> H(true);
    H.add_edge(1, 2, std::string("friendship"));
    H.add_edge(2, 3, std::string("colleague"));

    // you can still do BFS/DFS on H
    auto bfsH = graph_algo::bfs(H, 1);
    std::cout << "BFS on H from 1: ";
    for (auto id : bfsH) std::cout << id << " ";
    std::cout << "\n\n";
}

void test_graph_usecase3() {
    // ########## use case 3
    Graph<std::string, int, std::string> g3(false);

    // Add node directly by id and value:
    auto myNode = std::make_shared<Node<std::string,int>>(42, "custom");
    g3.add_node(myNode);  // uses the new overload (shared_ptr)
    std::cout << "Added external node: " << myNode->id() << " -> " << myNode->value() << "\n\n";
}

void test_graph_usecase4() {
    // ########## use case 4
    Graph<std::string, int, EdgeInfo> g4(false);

    // 1) Add 15 nodes with id 1..15 and a string payload
    for (int id = 1; id <= 15; ++id) {
        g4.add_node(id, std::string("Node") + std::to_string(id));
    }

    // 2) Define edges (from, to, EdgeInfo)
    std::vector<std::tuple<int,int,EdgeInfo>> edges = {
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
        int u = std::get<0>(t);
        int v = std::get<1>(t);
        EdgeInfo info = std::get<2>(t);
        g4.add_edge(u, v, info);
    }

    // 4) Print basic info
    std::cout << "Graph: nodes=" << g4.node_count()
              << ", edges=" << g4.edge_count() << "\n\n";

    for (auto &[id, name] : g4.list_nodes())
        std::cout << "Node " << id << ": " << name << "\n";

    // List all edges
    for (auto &[u, v, prop] : g4.list_edges())
        std::cout << u << " -> " << v << " : " << prop << "\n";

    std::cout << "\n";

    // 5) BFS from node 1 (now as free function)
    auto bfs_order = graph_algo::bfs(g4, 1);
    std::cout << "BFS order from 1: ";
    for (int id : bfs_order) std::cout << id << " ";
    std::cout << "\n\n";

    // 6) Dijkstra using extractor that reads weight from EdgeInfo (free function)
    auto extractor2 = [](const EdgeInfo &e)->double {
        // Optionally ignore inactive edges by returning a huge weight
        if (!e.active) return std::numeric_limits<double>::infinity();
        return e.weight;
    };

    auto [distances, prev] = graph_algo::dijkstra(g4, 1, extractor2);

    // Print distances
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Distances from 1 (using EdgeInfo.weight, inactive edges = unreachable):\n";
    for (int id = 1; id <= 15; ++id) {
        double d = distances[id];
        if (d == std::numeric_limits<double>::infinity()) {
            std::cout << id << ": unreachable\n";
        } else {
            std::cout << id << ": " << d << "\n";
        }
    }

    // Reconstruct and print path to a node (example: to node 14)
    int target = 14;
    // specify graph type so reconstruct_path can deduce id_type
    using MyGraph = Graph<std::string, int, EdgeInfo>;
    auto path = graph_algo::reconstruct_path<MyGraph>(prev, target);

    if (path.empty() || distances[target] == std::numeric_limits<double>::infinity()) {
        std::cout << "\nNo path (reachable) found from 1 to " << target << "\n";
    } else {
        std::cout << "\nPath 1 -> " << target << " (by ids): ";
        for (auto id : path) std::cout << id << " ";
        std::cout << "\nDistance: " << distances[target] << "\n";
    }

    std::cout << "\nMermaid syntax:\n" << g4.to_mermaid() << "\n";
}

// ----------------- main -----------------
int main() {
    test_sortings();
    test_graph_usecase1();
    test_graph_usecase2();
    test_graph_usecase3();
    test_graph_usecase4();
    return 0;
}
