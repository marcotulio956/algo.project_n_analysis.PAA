#include "usecases/graphs/usebipartite.hpp"
#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

using namespace std;
using namespace graph_algo;

void use_check_bipartite() {
    cout << "*** use_check_bipartite() ***\n";
    // Example graph: 4 nodes, edges: 1-2, 2-3, 3-4, 4-1 -> even cycle => bipartite
    Graph<string,int,string> g(true); // directed or not doesn't matter; algorithm uses undirected view
    for (int i=1;i<=4;++i) g.add_node(i, "N"+to_string(i));
    g.add_edge(1,2,"e12"); g.add_edge(2,3,"e23");
    g.add_edge(3,4,"e34"); g.add_edge(4,1,"e41");

    bool bip = is_bipartite(g);
    cout << "Graph 1 is bipartite? " << (bip ? "YES":"NO") << "\n";

    // Triangle -> non-bipartite
    Graph<string,int,string> g2(true);
    for (int i=1;i<=3;++i) g2.add_node(i, "N"+to_string(i));
    g2.add_edge(1,2,""); g2.add_edge(2,3,""); g2.add_edge(3,1,"");
    cout << "Graph 2 (triangle) is bipartite? " << (is_bipartite(g2) ? "YES":"NO") << "\n\n";
}
