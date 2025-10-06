#pragma once
#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <limits>
#include <tuple>
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include <functional>


// Free functions for algorithms that operate on a Graph-like type G.
// G must provide:
//   - using id_type = ...;
//   - using edge_property_type = ...;
//   - bool has_node(const id_type&) const;
//   - std::vector<std::pair<id_type, edge_property_type>> neighbors(const id_type&) const;
//   - std::vector<std::pair<id_type, typename G::value_type>> list_nodes() const;
//   - std::vector<std::tuple<id_type, id_type, edge_property_type>> list_edges() const;
//   - bool directed() const;

namespace graph_algo {

// ------------------ BFS ------------------
template <typename G>
std::vector<typename G::id_type> bfs(const G &g, const typename G::id_type &start) {
    using id_type = typename G::id_type;
    if (!g.has_node(start)) return {};

    std::vector<id_type> order;
    std::unordered_map<id_type, bool> visited;
    std::queue<id_type> q;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        id_type u = q.front(); q.pop();
        order.push_back(u);

        for (const auto &e : g.neighbors(u)) {
            const id_type &v = e.first;
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    return order;
}

// ------------------ DFS (iterative) ------------------
template <typename G>
std::vector<typename G::id_type> dfs(const G &g, const typename G::id_type &start) {
    using id_type = typename G::id_type;
    if (!g.has_node(start)) return {};

    std::vector<id_type> order;
    std::unordered_map<id_type, bool> visited;
    std::stack<id_type> st;

    st.push(start);
    while (!st.empty()) {
        id_type u = st.top(); st.pop();
        if (visited[u]) continue;
        visited[u] = true;
        order.push_back(u);

        // push neighbors in reverse so order close to recursive DFS
        auto neigh = g.neighbors(u);
        for (auto it = neigh.rbegin(); it != neigh.rend(); ++it) {
            if (!visited[it->first]) st.push(it->first);
        }
    }
    return order;
}

// ------------------ Dijkstra (generic extractor) ------------------
// dijkstra_with_extractor: user provides Extractor(edge_property) -> numeric Weight
template <typename G, typename Weight, typename Extractor>
std::pair<std::unordered_map<typename G::id_type, Weight>,
          std::unordered_map<typename G::id_type, std::optional<typename G::id_type>>>
dijkstra_with_extractor(const G &g, const typename G::id_type &start, Extractor extractor)
{
    using id_type = typename G::id_type;
    static_assert(std::is_arithmetic<Weight>::value, "Weight must be arithmetic");

    if (!g.has_node(start)) throw std::invalid_argument("start node doesn't exist");

    const Weight INF = std::numeric_limits<Weight>::infinity();
    std::unordered_map<id_type, Weight> dist;
    std::unordered_map<id_type, std::optional<id_type>> prev;

    // initialize using list_nodes()
    for (const auto &p : g.list_nodes()) {
        dist[p.first] = INF;
        prev[p.first] = std::nullopt;
    }
    dist[start] = Weight{0};

    using PQItem = std::pair<Weight, id_type>;
    auto cmp = [](const PQItem &a, const PQItem &b){ return a.first > b.first; };
    std::priority_queue<PQItem, std::vector<PQItem>, decltype(cmp)> pq(cmp);
    pq.push({dist[start], start});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (const auto &e : g.neighbors(u)) {
            id_type v = e.first;
            Weight w = static_cast<Weight>(extractor(e.second));
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    return {std::move(dist), std::move(prev)};
}

// Convenience wrapper that infers Weight from extractor return type
template <typename G, typename Extractor>
auto dijkstra(const G &g, const typename G::id_type &start, Extractor extractor)
    -> std::pair<
        std::unordered_map<typename G::id_type, std::decay_t<decltype(extractor(std::declval<typename G::edge_property_type>()))>>,
        std::unordered_map<typename G::id_type, std::optional<typename G::id_type>>
    >
{
    using Ret = std::decay_t<decltype(extractor(std::declval<typename G::edge_property_type>()))>;
    static_assert(std::is_arithmetic<Ret>::value, "Extractor must return arithmetic weight");
    return dijkstra_with_extractor<G, Ret, Extractor>(g, start, extractor);
}

// ------------------ Reconstruct path ------------------
template <typename G>
std::vector<typename G::id_type>
reconstruct_path(const std::unordered_map<typename G::id_type, std::optional<typename G::id_type>> &prev,
                 const typename G::id_type &target)
{
    using id_type = typename G::id_type;
    std::vector<id_type> path;
    auto it = prev.find(target);
    if (it == prev.end()) return path;
    if (!it->second.has_value()) return path; // unreachable or start (caller can inspect distances)

    id_type cur = target;
    while (true) {
        path.push_back(cur);
        auto pit = prev.find(cur);
        if (pit == prev.end() || !pit->second.has_value()) break;
        cur = pit->second.value();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// ------------------ Topological sort (Kahn) ------------------
template <typename G>
std::vector<typename G::id_type> topological_sort(const G &g) {
    using id_type = typename G::id_type;
    if (!g.directed()) throw std::logic_error("Topological sort requires a directed graph");

    std::unordered_map<id_type, int> indeg;
    for (const auto &p : g.list_nodes()) indeg[p.first] = 0;
    for (const auto &e : g.list_edges()) {
        id_type to = std::get<1>(e);
        indeg[to]++; 
    }

    std::queue<id_type> q;
    for (const auto &kv : indeg) if (kv.second == 0) q.push(kv.first);

    std::vector<id_type> order;
    while (!q.empty()) {
        id_type u = q.front(); q.pop();
        order.push_back(u);
        for (const auto &edge : g.neighbors(u)) {
            id_type v = edge.first;
            indeg[v]--;
            if (indeg[v] == 0) q.push(v);
        }
    }

    if (order.size() != indeg.size()) {
        throw std::runtime_error("Graph has at least one cycle (topo sort failed)");
    }
    return order;
}

// ---------- utility: pair hash for unordered_map keys ----------
template <typename A, typename B>
struct PairHash {
    std::size_t operator()(const std::pair<A,B>& p) const noexcept {
        // combine two hashes (works for most std::hash<A/B> specializations)
        std::size_t ha = std::hash<A>{}(p.first);
        std::size_t hb = std::hash<B>{}(p.second);
        // simple combination
        return ha ^ (hb + 0x9e3779b97f4a7c15ULL + (ha<<6) + (ha>>2));
    }
};

// -----------------------------------------------------------------
// Bipartite check (treats graph as undirected).
//    If graph is directed(), we form an undirected view from list_edges().
//    Returns true if graph is bipartite, false otherwise.
// -----------------------------------------------------------------
template <typename G>
bool is_bipartite(const G &g) {
    using id_type = typename G::id_type;

    // Build adjacency (undirected) view: unordered_map<id, vector<id>>
    std::unordered_map<id_type, std::vector<id_type>> adj;

    // initialize nodes (so isolated nodes appear)
    for (const auto &p : g.list_nodes()) adj[p.first] = {};

    // use list_edges to add both directions (works whether g is directed or not)
    for (const auto &e : g.list_edges()) {
        id_type u = std::get<0>(e);
        id_type v = std::get<1>(e);
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // color: -1 = uncolored, 0/1 = two colors
    std::unordered_map<id_type, int> color;
    color.reserve(adj.size());
    for (const auto &kv : adj) color[kv.first] = -1;

    std::queue<id_type> q;

    for (const auto &kv : adj) {
        id_type start = kv.first;
        if (color[start] != -1) continue;

        // start BFS coloring
        color[start] = 0;
        q.push(start);

        while (!q.empty()) {
            id_type u = q.front(); q.pop();
            for (auto v : adj[u]) {
                if (color[v] == -1) {
                    color[v] = color[u] ^ 1;
                    q.push(v);
                } else if (color[v] == color[u]) {
                    // same color on adjacent vertices -> not bipartite
                    return false;
                }
            }
        }
    }

    return true;
}

// -----------------------------------------------------------------
// Acyclic check
//    - For directed graphs: uses DFS color method to detect back-edges.
//    - For undirected graphs: uses DFS and parent check (detects cycles).
// -----------------------------------------------------------------
template <typename G>
bool is_acyclic(const G &g) {
    using id_type = typename G::id_type;

    // Use nodes list for initialization
    std::vector<id_type> nodes;
    nodes.reserve(g.list_nodes().size());
    for (const auto &p : g.list_nodes()) nodes.push_back(p.first);

    if (g.directed()) {
        // Directed cycle detection (colors: 0=white,1=gray,2=black)
        std::unordered_map<id_type, int> color;
        for (auto id : nodes) color[id] = 0;

        std::function<bool(const id_type&)> dfs_visit;
        dfs_visit = [&](const id_type &u) -> bool {
            color[u] = 1; // gray
            for (const auto &edge : g.neighbors(u)) {
                id_type v = edge.first;
                if (color[v] == 0) {
                    if (!dfs_visit(v)) return false;
                } else if (color[v] == 1) {
                    // back-edge found -> cycle
                    return false;
                }
            }
            color[u] = 2; // black
            return true;
        };

        for (auto id : nodes) {
            if (color[id] == 0) {
                if (!dfs_visit(id)) return false;
            }
        }
        return true;
    } else {
        // Undirected cycle detection using DFS with parent tracking
        std::unordered_map<id_type, bool> visited;
        for (auto id : nodes) visited[id] = false;

        std::function<bool(const id_type&, const id_type&)> dfs_undirected;
        dfs_undirected = [&](const id_type &u, const id_type &parent) -> bool {
            visited[u] = true;
            for (const auto &edge : g.neighbors(u)) {
                id_type v = edge.first;
                if (!visited[v]) {
                    if (!dfs_undirected(v, u)) return false;
                } else if (v != parent) {
                    // visited neighbor that's not parent -> cycle
                    return false;
                }
            }
            return true;
        };

        for (auto id : nodes) {
            if (!visited[id]) {
                if (!dfs_undirected(id, id)) return false;
            }
        }
        return true;
    }
}

// -----------------------------------------------------------------
// Kosaraju's algorithm for Strongly Connected Components (SCCs)
//    Works for directed graphs. For undirected graphs it will return
//    connected components (each SCC will equal a connected component).
//    Returns vector<vector<id_type>> where each inner vector is one component.
// -----------------------------------------------------------------
template <typename G>
std::vector<std::vector<typename G::id_type>> kosaraju_scc(const G &g) {
    using id_type = typename G::id_type;

    // 1) Build adjacency lists (directed)
    std::unordered_map<id_type, std::vector<id_type>> adj;
    std::unordered_map<id_type, std::vector<id_type>> rev_adj;

    for (const auto &p : g.list_nodes()) {
        adj[p.first] = {};
        rev_adj[p.first] = {};
    }

    for (const auto &e : g.list_edges()) {
        id_type u = std::get<0>(e);
        id_type v = std::get<1>(e);
        adj[u].push_back(v);
        rev_adj[v].push_back(u);
        // if graph is undirected, list_edges likely includes both directions or we should add reverse:
        if (!g.directed()) {
            rev_adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    // 2) First pass: order vertices by finish time (DFS)
    std::unordered_map<id_type, bool> visited;
    for (const auto &kv : adj) visited[kv.first] = false;
    std::vector<id_type> order;
    order.reserve(adj.size());

    std::function<void(const id_type&)> dfs1 = [&](const id_type &u) {
        visited[u] = true;
        for (auto v : adj[u])
            if (!visited[v]) dfs1(v);
        order.push_back(u); // push after visiting descendants
    };

    for (const auto &kv : adj) {
        if (!visited[kv.first]) dfs1(kv.first);
    }

    // 3) Second pass: DFS on reversed graph in reverse finish order
    for (auto &kv : visited) kv.second = false;
    std::vector<std::vector<id_type>> components;
    components.reserve(adj.size());

    std::function<void(const id_type&, std::vector<id_type>&)> dfs2 =
        [&](const id_type &u, std::vector<id_type> &comp) {
            visited[u] = true;
            comp.push_back(u);
            for (auto v : rev_adj[u])
                if (!visited[v]) dfs2(v, comp);
        };

    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        id_type u = *it;
        if (!visited[u]) {
            components.emplace_back();
            dfs2(u, components.back());
        }
    }

    return components;
}

// -----------------------------------------------------------------
// Max flow (Edmonds–Karp implementation - BFS augmenting paths)
//    Requires a capacity extractor: cap_extractor(edge_prop) -> Capacity (arithmetic).
//    The graph is treated as directed. If g is undirected, we add both directions
//    with the same capacity (i.e., treat each undirected edge as two directed edges).
// -----------------------------------------------------------------
template <typename G, typename CapacityExtractor>
auto edmonds_karp_maxflow(const G &g,
                          const typename G::id_type &source,
                          const typename G::id_type &sink,
                          CapacityExtractor cap_extractor)
    -> std::decay_t<decltype(cap_extractor(std::declval<typename G::edge_property_type>()))>
{
    using id_type = typename G::id_type;
    using capacity_type = std::decay_t<decltype(cap_extractor(std::declval<typename G::edge_property_type>()))>;
    static_assert(std::is_arithmetic<capacity_type>::value, "Capacity must be numeric");

    // Build node list and index presence
    std::unordered_set<id_type> nodes_set;
    for (const auto &p : g.list_nodes()) nodes_set.insert(p.first);

    if (!nodes_set.count(source) || !nodes_set.count(sink)) {
        throw std::invalid_argument("source or sink not present in graph");
    }

    // Residual capacity map: (u,v) -> capacity
    std::unordered_map<std::pair<id_type,id_type>, capacity_type, PairHash<id_type,id_type>> capacity;
    std::unordered_map<id_type, std::vector<id_type>> adj; // adjacency for residual graph (neighbors with possible residual > 0)

    // initialize adjacency entries
    for (auto id : nodes_set) adj[id] = {};

    // Initialize capacities from graph edges
    for (const auto &e : g.list_edges()) {
        id_type u = std::get<0>(e);
        id_type v = std::get<1>(e);
        capacity_type c = cap_extractor(std::get<2>(e));
        if (c < capacity_type(0)) {
            throw std::invalid_argument("negative capacity encountered");
        }
        // sum capacities when parallel edges exist
        capacity[{u,v}] = capacity[{u,v}] + c;
        // ensure reverse entry exists
        if (!capacity.count({v,u})) capacity[{v,u}] = capacity_type(0);

        // adjacency edges for residual graph (we'll keep all u->v and v->u present)
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Ensure every node has adjacency vector (including isolated)
    for (const auto &p : g.list_nodes()) {
        if (!adj.count(p.first)) adj[p.first] = {};
    }

    capacity_type flow = capacity_type(0);

    // BFS to find augmenting path; returns parent map (node->prev_node) if found, else empty
    auto bfs_find_path = [&](std::unordered_map<id_type, id_type> &parent) -> bool {
        parent.clear();
        std::queue<id_type> q;
        std::unordered_set<id_type> visited;
        q.push(source);
        visited.insert(source);

        while (!q.empty()) {
            id_type u = q.front(); q.pop();
            for (id_type v : adj[u]) {
                // residual capacity u->v
                capacity_type residual = capacity[{u,v}];
                if (residual > capacity_type(0) && !visited.count(v)) {
                    parent[v] = u;
                    if (v == sink) return true;
                    visited.insert(v);
                    q.push(v);
                }
            }
        }
        return false;
    };

    std::unordered_map<id_type, id_type> parent;
    while (bfs_find_path(parent)) {
        // find bottleneck along the path sink <- ... <- source
        capacity_type bottleneck = std::numeric_limits<capacity_type>::infinity();
        id_type v = sink;
        while (v != source) {
            id_type u = parent[v];
            capacity_type residual = capacity[{u,v}];
            if (residual < bottleneck) bottleneck = residual;
            v = u;
        }
        if (bottleneck == std::numeric_limits<capacity_type>::infinity()) break; // defensive

        // apply flow along the path
        v = sink;
        while (v != source) {
            id_type u = parent[v];
            capacity[{u,v}] -= bottleneck;
            capacity[{v,u}] += bottleneck; // reverse edge increases residual capacity
            v = u;
        }
        flow += bottleneck;
    }

    return flow;
}

// -----------------------------------------------------------------
// Bellman-Ford algorithm + negative-cycle detection
//    - Returns tuple: (dist_map, prev_map, has_negative_cycle)
//    - Extractor(edge_prop) -> Weight (arithmetic). Negative weights allowed.
// -----------------------------------------------------------------
template <typename G, typename Extractor>
auto bellman_ford(const G &g,
                  const typename G::id_type &start,
                  Extractor extractor)
    -> std::tuple<
           std::unordered_map<typename G::id_type, std::decay_t<decltype(extractor(std::declval<typename G::edge_property_type>()))>>,
           std::unordered_map<typename G::id_type, std::optional<typename G::id_type>>,
           bool
       >
{
    using id_type = typename G::id_type;
    using Weight = std::decay_t<decltype(extractor(std::declval<typename G::edge_property_type>()))>;
    static_assert(std::is_arithmetic<Weight>::value, "Extractor must return arithmetic weight");

    const Weight INF = std::numeric_limits<Weight>::infinity();

    // initialize distances & prev
    std::unordered_map<id_type, Weight> dist;
    std::unordered_map<id_type, std::optional<id_type>> prev;
    for (const auto &p : g.list_nodes()) {
        dist[p.first] = INF;
        prev[p.first] = std::nullopt;
    }
    if (!g.has_node(start)) throw std::invalid_argument("start node doesn't exist");
    dist[start] = Weight{0};

    // Relax edges n-1 times
    size_t n = g.list_nodes().size();
    for (size_t i = 0; i + 1 < n; ++i) {
        bool updated = false;
        for (const auto &e : g.list_edges()) {
            id_type u = std::get<0>(e);
            id_type v = std::get<1>(e);
            Weight w = extractor(std::get<2>(e));
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                updated = true;
            }
            // note: if graph is undirected and list_edges contains single undirected entry,
            // relax does only one direction; if list_edges has both directions, both will be relaxed.
        }
        if (!updated) break;
    }

    // Check for negative-weight cycles: if we can relax any edge further, there's a negative cycle
    bool has_negative = false;
    for (const auto &e : g.list_edges()) {
        id_type u = std::get<0>(e);
        id_type v = std::get<1>(e);
        Weight w = extractor(std::get<2>(e));
        if (dist[u] != INF && dist[u] + w < dist[v]) {
            has_negative = true;
            break;
        }
    }

    return {std::move(dist), std::move(prev), has_negative};
}

// Convenience wrapper: check only boolean whether negative cycle exists anywhere
template <typename G, typename Extractor>
bool has_negative_cycle(const G &g, Extractor extractor) {
    using id_type = typename G::id_type;
    // choose arbitrary start; run Bellman-Ford from each connected component (safer)
    // We'll run Bellman-Ford with artificial super-source connecting to all nodes with 0 weight
    // to detect negative cycles anywhere. To avoid changing Graph, do repeated BF from each node:
    for (const auto &p : g.list_nodes()) {
        auto [dist, prev, has_neg] = bellman_ford<G, Extractor>(g, p.first, extractor);
        if (has_neg) return true;
    }
    return false;
}

} // namespace graph_algo

#endif // GRAPH_ALGORITHMS_HPP
