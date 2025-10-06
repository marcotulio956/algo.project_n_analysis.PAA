#include "usecases/graphs/usekosaraju.hpp"
#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

using namespace std;
using namespace graph_algo;

void use_kosaraju_scc() {
    cout << "*** use_kosaraju_scc() ***\n";
    // nodes {1,2,3,4}
    // edges: 1<->2 (SCC {1,2}), 3<->4 (SCC {3,4})
    Graph<string,int,string> g(true);
    for (int i=1;i<=4;++i) g.add_node(i,"N"+to_string(i));
    g.add_edge(1,2,""); g.add_edge(2,1,"");
    g.add_edge(3,4,""); g.add_edge(4,3,"");

    auto comps = kosaraju_scc(g);
    cout << "Found " << comps.size() << " components:\n";
    for (size_t i=0;i<comps.size();++i) {
        cout << "  Component " << i << " : ";
        for (auto id : comps[i]) cout << id << " ";
        cout << "\n";
    }
    cout << "\n";
}
