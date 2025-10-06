#include "usecases/graphs/usegraph.hpp"
#include "util.hpp"
#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

void use_graph_case1(){
    // ########## use case 1
    struct W { double w; }; // edge property with a numeric field

    Graph<string, int, W> g1(true);
    g1.add_node(1, "A");
    g1.add_node(2, "B");
    g1.add_edge(1, 2, W{2.5});

    auto extractor1 = [](const W& p)->double { return p.w; }; // extractor for edge weight

    // Use free function from graph_algorithms.hpp
    auto [dist1, prev1] = graph_algo::dijkstra(g1, 1, extractor1);

    // print distances (example)
    cout << "Distances from 1 in g1:\n";
    for (const auto &p : g1.list_nodes()) {
        int id = p.first;
        double d = dist1[id];
        if (d == numeric_limits<double>::infinity()) cout << id << ": unreachable\n";
        else cout << id << ": " << d << "\n";
    }
    cout << "\n";
}

void use_graph_case2(){
    // ########## use case 2
    struct EdgeInfo2 { string name; double cost; int capacity; };

    Graph<int, int, EdgeInfo2> g2(false);
    g2.add_edge(1, 2, {"road-12", 3.5, 100});

    auto get_cost = [](const EdgeInfo2& e){ return e.cost; };
    auto [dist2, prev2] = graph_algo::dijkstra(g2, 1, get_cost);

    // Graph with only label as edges with no numerical weight
    Graph<int, int, string> H(true);
    H.add_edge(1, 2, string("friendship"));
    H.add_edge(2, 3, string("colleague"));

    // you can still do BFS/DFS on H
    auto bfsH = graph_algo::bfs(H, 1);
    cout << "BFS on H from 1: ";
    for (auto id : bfsH) cout << id << " ";
    cout << "\n\n";
}

void use_graph_case3() {
    // ########## use case 3
    Graph<string, int, string> g3(false);

    // Add node directly by id and value:
    auto myNode = make_shared<Node<string,int>>(42, "custom");
    g3.add_node(myNode);  // uses the new overload (shared_ptr)
    cout << "Added external node: " << myNode->id() << " -> " << myNode->value() << "\n\n";
}