#include "usecases/graphs/useacyclic.hpp"
#include <iostream>
#include <string>

#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"

using namespace std;
using namespace graph_algo;

void use_check_acyclic() {
    cout << "*** use_check_acyclic() ***\n";

    // Directed acyclic graph (DAG)
    Graph<string,int,string> dag(true);
    dag.add_node(1,"A"); dag.add_node(2,"B"); dag.add_node(3,"C");
    dag.add_edge(1,2,""); dag.add_edge(2,3,""); // 1->2->3, no back edges
    cout << "DAG is acyclic? " << (is_acyclic(dag) ? "YES":"NO") << "\n";

    // Directed cyclic graph
    Graph<string,int,string> cyc(true);
    cyc.add_node(1,"A"); cyc.add_node(2,"B"); cyc.add_node(3,"C");
    cyc.add_edge(1,2,""); cyc.add_edge(2,3,""); cyc.add_edge(3,1,""); // cycle
    cout << "Cyclic graph is acyclic? " << (is_acyclic(cyc) ? "YES":"NO") << "\n";

    // Undirected triangle
    Graph<string,int,string> und(false);
    und.add_node(1,"A"); und.add_node(2,"B"); und.add_node(3,"C");
    und.add_edge(1,2,""); und.add_edge(2,3,""); und.add_edge(3,1,""); // triangle
    cout << "Undirected triangle considered acyclic? " << (is_acyclic(und) ? "YES":"NO") << "\n\n";
}
