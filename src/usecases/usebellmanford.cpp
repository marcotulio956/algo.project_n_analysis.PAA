#include "usecases/graphs/usebellmanford.hpp"
#include <iostream>
#include <string>
#include <limits>

#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

using namespace std;
using namespace graph_algo;

void use_bellman_ford_and_negative_cycle() {
    cout << "*** use_bellman_ford_and_negative_cycle() ***\n";

    // Example 1: no negative cycle
    struct W { double w; };
    Graph<string,int,W> g1(true);
    for (int i=1;i<=4;++i) g1.add_node(i,"N"+to_string(i));
    g1.add_edge(1,2,W{1});
    g1.add_edge(2,3,W{2});
    g1.add_edge(1,3,W{4});
    g1.add_edge(3,4,W{-5});

    auto extractor = [](const W &p)->double { return p.w; };
    auto [dist1, prev1, neg1] = bellman_ford(g1, 1, extractor);
    cout << "Graph g1 has negative cycle? " << (neg1 ? "YES":"NO") << "\n";
    cout << "Distances from 1:\n";
    for (const auto &p : g1.list_nodes()) {
        int id = p.first;
        double d = dist1[id];
        if (d == std::numeric_limits<double>::infinity()) cout << id << ": unreachable\n";
        else cout << id << ": " << d << "\n";
    }
    cout << "\n";

    // Example 2: negative cycle
    Graph<string,int,W> g2(true);
    g2.add_node(1,"A"); g2.add_node(2,"B"); g2.add_node(3,"C");
    g2.add_edge(1,2,W{1}); g2.add_edge(2,3,W{-2}); g2.add_edge(3,1,W{-1});
    auto [dist2, prev2, neg2] = bellman_ford(g2, 1, extractor);
    cout << "Graph g2 has negative cycle? " << (neg2 ? "YES":"NO") << "\n\n";

    bool any_neg = has_negative_cycle(g2, extractor);
    cout << "has_negative_cycle(g2) -> " << (any_neg ? "YES":"NO") << "\n\n";
}
