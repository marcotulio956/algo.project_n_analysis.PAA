#include "usecases/graphs/usemaxflow.hpp"
#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

using namespace std;
using namespace graph_algo;

void use_edmonds_karp_maxflow() {
    cout << "*** use_edmonds_karp_maxflow() ***\n";
    // Classic example:
    // 1(s) -> 2 (3), 1->3 (2), 2->3 (1), 2->4 (2), 3->4 (4)
    struct CapInfo { int cap; };
    Graph<string,int,CapInfo> g(true);
    g.add_node(1,"s"); g.add_node(2,"a"); g.add_node(3,"b"); g.add_node(4,"t");
    g.add_edge(1,2, CapInfo{3});
    g.add_edge(1,3, CapInfo{2});
    g.add_edge(2,3, CapInfo{1});
    g.add_edge(2,4, CapInfo{2});
    g.add_edge(3,4, CapInfo{4});

    auto cap_extractor = [](const CapInfo &p)->int { return p.cap; };
    int maxflow = edmonds_karp_maxflow(g, 1, 4, cap_extractor);
    cout << "Max flow from 1->4 = " << maxflow << "\n\n";
}
