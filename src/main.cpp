// main.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <memory>
#include <tuple>
#include <string>

#include "sortings.hpp"
#include "graph.hpp"
#include "graph_algorithms.hpp"
#include "node.hpp"
#include "util.hpp"

#include "usecases/sortings/usesortings.hpp"
#include "usecases/graphs/usegraph.hpp"
#include "usecases/graphs/usedijkstra.hpp"
#include "usecases/graphs/usebipartite.hpp"
#include "usecases/graphs/useacyclic.hpp"
#include "usecases/graphs/usekosaraju.hpp"
#include "usecases/graphs/usemaxflow.hpp"
#include "usecases/graphs/usebellmanford.hpp"

using namespace std;
using namespace graph_algo;

// ----------------- main -----------------
int main() {
    use_sortings();
    
    use_graph_case1();
    use_graph_case2();
    use_graph_case3();
    
    use_check_bipartite();
    use_check_acyclic();
    use_kosaraju_scc();
    use_edmonds_karp_maxflow();
    use_bellman_ford_and_negative_cycle();
    return 0;
}
