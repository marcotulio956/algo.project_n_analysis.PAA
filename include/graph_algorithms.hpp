#pragma once
#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <utility>

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

} // namespace graph_algo

#endif // GRAPH_ALGORITHMS_HPP
