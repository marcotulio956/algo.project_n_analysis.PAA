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

using namespace std;
using namespace graph_algo;

// ----------------- helper types and operator<< -----------------
struct EdgeInfo {
    string label; // name/label of the edge
    double weight;     // numeric weight (for shortest-path)
    int capacity;      // arbitrary integer property
    bool active;       // boolean flag
};

// helper to print edge info
ostream& operator<<(ostream& os, const EdgeInfo& e) {
    os << "{label:" << e.label
       << ", weight:" << e.weight
       << ", cap:" << e.capacity
       << ", active:" << (e.active ? "Y" : "N") << "}";
    return os;
}

// ----------------- test functions -----------------
void test_sortings() {
    vector<int> arr = {64, 25, 12, 22, 11, 07, 90, 45, 23, 67, 89};
    cout << "original array: "; Util::print_vector(arr);
    Sortings::insertion_sort(arr);

    arr = {13, 46, 24, 52, 20, 9, 78, 34, 56, 12, 01};
    cout << "original array: "; Util::print_vector(arr);
    Sortings::selection_sort(arr, false);

    arr = {23, 45, 12, 67, 34, 89, 90, 11, 78, 56, 01};
    cout << "original array: "; Util::print_vector(arr);
    Sortings::merge_sort(arr);

    arr = {1, 2, 3};
    cout << "greatest: " << Sortings::find_greatest(arr) << endl;
    cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << endl;

    arr = {3 , 2, 1};
    cout << "greatest: " << Sortings::find_greatest(arr) << endl;
    cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << endl;

    arr = {3};
    cout << "greatest: " << Sortings::find_greatest(arr) << endl;
    cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << endl;

    arr = {3, 3, 3};
    cout << "greatest: " << Sortings::find_greatest(arr) << endl;
    cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << endl;

    arr = {3, 3, 2, 1, 1, 2};
    for (size_t i=0; i<arr.size(); i++) {
        cout<< "nth greatest: " << i+1 << " is " << Sortings::find_nthgreatest(arr, i+1) << endl;
    }
}

void test_graph_usecase1(){
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

void test_graph_usecase2(){
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

void test_graph_usecase3() {
    // ########## use case 3
    Graph<string, int, string> g3(false);

    // Add node directly by id and value:
    auto myNode = make_shared<Node<string,int>>(42, "custom");
    g3.add_node(myNode);  // uses the new overload (shared_ptr)
    cout << "Added external node: " << myNode->id() << " -> " << myNode->value() << "\n\n";
}

void test_graph_usecase4() {
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



void use_check_bipartite() {
    cout << "*** use_check_bipartite() ***\n";
    // Example graph: 4 nodes, edges: 1-2, 2-3, 3-4, 4-1 -> even cycle => bipartite
    Graph<string,int,string> g(true); // directed or not doesn't matter; algorithm uses undirected view
    for (int i=1;i<=4;++i) g.add_node(i, "N"+to_string(i));
    g.add_edge(1,2,"e12"); g.add_edge(2,3,"e23");
    g.add_edge(3,4,"e34"); g.add_edge(4,1,"e41");

    bool bip = is_bipartite(g);
    cout << "Graph 1 is bipartite? " << (bip ? "YES":"NO") << "\n";

    // Add an odd cycle (triangle) to make it non-bipartite
    Graph<string,int,string> g2(true);
    for (int i=1;i<=3;++i) g2.add_node(i, "N"+to_string(i));
    g2.add_edge(1,2,""); g2.add_edge(2,3,""); g2.add_edge(3,1,"");
    cout << "Graph 2 (triangle) is bipartite? " << (is_bipartite(g2) ? "YES":"NO") << "\n\n";
}


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
    cyc.add_edge(1,2,""); cyc.add_edge(2,3,""); cyc.add_edge(3,1,""); // cycle 1->2->3->1
    cout << "Cyclic graph is acyclic? " << (is_acyclic(cyc) ? "YES":"NO") << "\n";

    // Undirected cycle example
    Graph<string,int,string> und(true); // will be treated as undirected in check
    und.add_node(1,"A"); und.add_node(2,"B"); und.add_node(3,"C");
    und.add_edge(1,2,""); und.add_edge(2,3,""); und.add_edge(3,1,""); // triangle
    cout << "Undirected triangle considered acyclic? " << (is_acyclic(und) ? "YES":"NO") << "\n\n";
}

void use_kosaraju_scc() {
    cout << "*** use_kosaraju_scc() ***\n";
    // Example: nodes {1,2,3,4}
    // edges: 1->2, 2->1 (SCC {1,2}); 3->4 (SCC {3}); 4->3 (SCC {3,4})
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

void use_edmonds_karp_maxflow() {
    cout << "*** use_edmonds_karp_maxflow() ***\n";

    // We'll use a small classic example:
    // Node ids: 1 (s), 2, 3, 4 (t)
    // Edges with capacities:
    // 1->2 (3), 1->3 (2), 2->3 (1), 2->4 (2), 3->4 (4)
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


void use_bellman_ford_and_negative_cycle() {
    cout << "*** use_bellman_ford_and_negative_cycle() ***\n";

    // Example 1: graph with negative edge weights but no negative cycle
    struct W { double w; };
    Graph<string,int,W> g1(true);
    for (int i=1;i<=4;++i) g1.add_node(i,"N"+to_string(i));
    // edges: 1->2 (1), 2->3 (2), 1->3 (4), 3->4 (-5)
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
        if (d == numeric_limits<double>::infinity()) cout << id << ": unreachable\n";
        else cout << id << ": " << d << "\n";
    }
    cout << "\n";

    // Example 2: graph with a negative cycle: 1->2 (1), 2->3 (-2), 3->1 (-1) => cycle weight -2
    Graph<string,int,W> g2(true);
    g2.add_node(1,"A"); g2.add_node(2,"B"); g2.add_node(3,"C");
    g2.add_edge(1,2,W{1}); g2.add_edge(2,3,W{-2}); g2.add_edge(3,1,W{-1});
    auto [dist2, prev2, neg2] = bellman_ford(g2, 1, extractor);
    cout << "Graph g2 has negative cycle? " << (neg2 ? "YES":"NO") << "\n\n";

    // Global negative cycle test (anywhere in graph) - convenience wrapper (runs BF from each node)
    bool any_neg = has_negative_cycle(g2, extractor);
    cout << "has_negative_cycle(g2) -> " << (any_neg ? "YES":"NO") << "\n\n";
}

// ----------------- main -----------------
int main() {
    // test_sortings();
    // test_graph_usecase1();
    // test_graph_usecase2();
    // test_graph_usecase3();
    // test_graph_usecase4();
    use_check_bipartite();
    use_check_acyclic();
    use_kosaraju_scc();
    use_edmonds_karp_maxflow();
    use_bellman_ford_and_negative_cycle();
    return 0;
}
