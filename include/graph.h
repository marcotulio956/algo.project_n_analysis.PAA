#pragma once
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Node.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <optional>
#include <type_traits>
#include <memory>
#include <variant> // for std::monostate if default edge prop used
#include <utility>

/*
 * Graph<T, Id, EdgeProp>
 *
 * - T : payload type stored in nodes
 * - Id: identifier type used as node key (default size_t)
 * - EdgeProp: arbitrary property type stored in each edge (default std::monostate)
 *
 * Key changes from a numeric-weight-only graph:
 *  - Edges store an EdgeProp object (could be a number, string name, struct, etc.)
 *  - Algorithms that need numeric weights (e.g. Dijkstra) receive a user-provided
 *    extractor function: Weight extractor(const EdgeProp&).
 *    The extractor's return type must be arithmetic.
 *
 * Adjacency: unordered_map<Id, vector<pair<Id, EdgeProp>>>
 */

template <
    typename T,
    typename Id = std::size_t,
    typename EdgeProp = std::monostate
>
class Graph {
public:
    using id_type = Id;
    using value_type = T;
    using edge_property_type = EdgeProp;
    using node_type = Node<value_type, id_type>;
    using node_ptr  = std::shared_ptr<node_type>;

    // Edge stored as (neighbor id, edge_property)
    using Edge = std::pair<id_type, edge_property_type>;
    using AdjacencyList = std::unordered_map<id_type, std::vector<Edge>>;

    Graph(bool directed = false) : directed_(directed) {}

    // ---------- Node operations ----------
    void add_node(const id_type& id) {
        add_node(id, value_type{});
    }

    void add_node(const id_type& id, const value_type& value) {
        auto it = nodes_.find(id);
        if (it == nodes_.end()) {
            node_ptr n = std::make_shared<node_type>(id, value);
            nodes_.emplace(id, n);
            adj_.emplace(id, std::vector<Edge>{});
        } else {
            it->second->set_value(value);
        }
    }

    void add_node(const node_ptr& n) {
        nodes_[n->id()] = n;
        adj_.emplace(n->id(), std::vector<Edge>{});
    }

    void add_node(const id_type& id, value_type&& value) {
        auto it = nodes_.find(id);
        if (it == nodes_.end()) {
            node_ptr n = std::make_shared<node_type>(id, std::move(value));
            nodes_.emplace(id, n);
            adj_.emplace(id, std::vector<Edge>{});
        } else {
            it->second->set_value(std::move(value));
        }
    }

    bool has_node(const id_type& id) const noexcept {
        return nodes_.find(id) != nodes_.end();
    }

    bool remove_node(const id_type& id) {
        if (!has_node(id)) return false;
        adj_.erase(id);
        for (auto &kv : adj_) {
            auto &vec = kv.second;
            vec.erase(std::remove_if(vec.begin(), vec.end(),
                                     [&](const Edge& e){ return e.first == id; }),
                      vec.end());
        }
        nodes_.erase(id);
        return true;
    }

    node_ptr get_node(const id_type& id) const {
        auto it = nodes_.find(id);
        return it == nodes_.end() ? nullptr : it->second;
    }

    // ---------- Edge operations ----------
    // Add edge with a property. If nodes missing, they are created with default node value.
    void add_edge(const id_type& from, const id_type& to, edge_property_type prop = edge_property_type{}) {
        if (!has_node(from)) add_node(from);
        if (!has_node(to)) add_node(to);
        adj_[from].emplace_back(to, prop);
        if (!directed_) {
            adj_[to].emplace_back(from, prop);
        }
    }

    // Remove edge (for undirected graphs removes both directions)
    bool remove_edge(const id_type& from, const id_type& to) {
        if (!has_node(from) || !has_node(to)) return false;
        bool removed = remove_edge_internal(from, to);
        if (!directed_) {
            removed = remove_edge_internal(to, from) || removed;
        }
        return removed;
    }

    // Return neighbors (copy)
    std::vector<Edge> neighbors(const id_type& id) const {
        auto it = adj_.find(id);
        if (it == adj_.end()) return {};
        return it->second;
    }

    // Find edges (all outgoing from 'from' to 'to') — returns indices and properties
    std::vector<edge_property_type> find_edge_props(const id_type& from, const id_type& to) const {
        std::vector<edge_property_type> out;
        auto it = adj_.find(from);
        if (it == adj_.end()) return out;
        for (const auto &e : it->second) if (e.first == to) out.push_back(e.second);
        return out;
    }

    // ---------- Traversals ----------
    std::vector<id_type> bfs(const id_type& start) const {
        if (!has_node(start)) return {};
        std::vector<id_type> order;
        std::unordered_map<id_type, bool> visited;
        std::queue<id_type> q;
        visited[start] = true;
        q.push(start);
        while (!q.empty()) {
            auto u = q.front(); q.pop();
            order.push_back(u);
            for (const auto &e : neighbors(u)) {
                if (!visited[e.first]) {
                    visited[e.first] = true;
                    q.push(e.first);
                }
            }
        }
        return order;
    }

    std::vector<id_type> dfs(const id_type& start) const {
        if (!has_node(start)) return {};
        std::vector<id_type> order;
        std::unordered_map<id_type, bool> visited;
        std::stack<id_type> st;
        st.push(start);
        while (!st.empty()) {
            id_type u = st.top(); st.pop();
            if (visited[u]) continue;
            visited[u] = true;
            order.push_back(u);
            auto neigh = neighbors(u);
            for (auto it = neigh.rbegin(); it != neigh.rend(); ++it) {
                if (!visited[it->first]) st.push(it->first);
            }
        }
        return order;
    }

    // ---------- Shortest paths (Dijkstra) ----------
    // Generic Dijkstra that requires a weight-extractor:
    //
    //   Weight extractor(const EdgeProp& prop)
    //
    // The extractor's return type (Weight) must be an arithmetic type.
    //
    // Returns: pair of (distances map, predecessors map)
    template <typename Weight, typename Extractor>
    std::pair<std::unordered_map<id_type, Weight>,
              std::unordered_map<id_type, std::optional<id_type>>>
    dijkstra_with_extractor(const id_type& start, Extractor extractor) const
    {
        static_assert(std::is_arithmetic<Weight>::value, "Weight must be an arithmetic type");
        if (!has_node(start)) throw std::invalid_argument("start node doesn't exist");

        const Weight INF = std::numeric_limits<Weight>::infinity();
        std::unordered_map<id_type, Weight> dist;
        std::unordered_map<id_type, std::optional<id_type>> prev;

        for (const auto &kv : nodes_) {
            dist[kv.first] = INF;
            prev[kv.first] = std::nullopt;
        }
        dist[start] = Weight{0};

        using PQItem = std::pair<Weight, id_type>;
        auto cmp = [](const PQItem& a, const PQItem& b){ return a.first > b.first; };
        std::priority_queue<PQItem, std::vector<PQItem>, decltype(cmp)> pq(cmp);
        pq.push({dist[start], start});

        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            for (const auto &e : neighbors(u)) {
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

    // Helper that infers Weight from extractor's return type (convenience wrapper).
    template <typename Extractor>
    auto dijkstra(const id_type& start, Extractor extractor) const
        -> std::pair<
            std::unordered_map<id_type, std::decay_t<decltype(extractor(std::declval<edge_property_type>()))>>,
            std::unordered_map<id_type, std::optional<id_type>>
        >
    {
        using Ret = std::decay_t<decltype(extractor(std::declval<edge_property_type>()))>;
        static_assert(std::is_arithmetic<Ret>::value, "Extractor must return an arithmetic weight");
        return dijkstra_with_extractor<Ret, Extractor>(start, extractor);
    }

    // Reconstruct path produced by dijkstra prev map (returns empty vector if no path)
    std::vector<id_type> reconstruct_path(
            const std::unordered_map<id_type, std::optional<id_type>>& prev,
            const id_type& target) const
    {
        std::vector<id_type> path;
        auto it = prev.find(target);
        if (it == prev.end()) return path;
        // if target is unreachable, prev[target] stays nullopt. Caller should check distances map.
        if (!it->second.has_value()) {
            // could be start node or unreachable; return {target} only if it's the start and prev==nullopt
            // We'll return empty here; caller can decide based on dist map.
            return path;
        }

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

    // ---------- Topological sort ----------
    std::vector<id_type> topological_sort() const {
        if (!directed_) {
            throw std::logic_error("Topological sort requires a directed graph");
        }
        std::unordered_map<id_type, int> indeg;
        for (const auto &kv : nodes_) indeg[kv.first] = 0;
        for (const auto &kv : adj_) {
            for (const auto &e : kv.second) {
                indeg[e.first]++;
            }
        }
        std::queue<id_type> q;
        for (const auto &kv : indeg) if (kv.second == 0) q.push(kv.first);

        std::vector<id_type> order;
        while (!q.empty()) {
            id_type u = q.front(); q.pop();
            order.push_back(u);
            for (const auto &e : neighbors(u)) {
                indeg[e.first]--;
                if (indeg[e.first] == 0) q.push(e.first);
            }
        }

        if (order.size() != nodes_.size()) {
            throw std::runtime_error("Graph has at least one cycle (topo sort failed)");
        }
        return order;
    }

    // ---------- Utility ----------
    std::size_t node_count() const noexcept { return nodes_.size(); }
    std::size_t edge_count() const noexcept {
        std::size_t sum = 0;
        for (const auto &kv : adj_) sum += kv.second.size();
        if (!directed_) sum /= 2;
        return sum;
    }

    bool directed() const noexcept { return directed_; }

    void clear() noexcept {
        nodes_.clear();
        adj_.clear();
    }

    // ------------------------------------------------------------------
    // Return a string with Mermaid syntax for visualization
    // ------------------------------------------------------------------
    std::string to_mermaid(bool directed_style = false) const {
        std::ostringstream out;
        out << "graph " << (directed_style ? "LR" : "LR") << "\n";

        // 1) List all nodes
        for (const auto &pair : nodes_) {
            const auto &node = pair.second;
            out << "  N" << node->id() << "[\"" << node->value() << "\"]\n";
        }
        out << "\n";

        // 2) List all edges (avoid duplicates for undirected graphs)
        std::unordered_set<std::string> seen;
        for (const auto &[from, vec] : adj_) {
            for (const auto &[to, prop] : vec) {
                if (!directed_ && from > to) continue; // avoid duplicates

                std::ostringstream key;
                key << from << "-" << to;
                if (seen.count(key.str())) continue;
                seen.insert(key.str());

                // print edge with all property fields (requires operator<< for EdgeProp)
                out << "  N" << from 
                    << (directed_ ? " -->|" : " --|")
                    << prop << "|--> "
                    << "N" << to << "\n";
            }
        }

        return out.str();
    }

    // ------------------------------------------------------------------
    // Return a list of all nodes (id, value)
    // ------------------------------------------------------------------
    std::vector<std::pair<Id, T>> list_nodes() const {
        std::vector<std::pair<Id, T>> result;
        result.reserve(nodes_.size());
        for (const auto &[id, node] : nodes_) {
            result.emplace_back(id, node->value());
        }
        return result;
    }

    // ------------------------------------------------------------------
    // Return a list of all edges (from, to, prop)
    // ------------------------------------------------------------------
    std::vector<std::tuple<Id, Id, EdgeProp>> list_edges() const {
        std::vector<std::tuple<Id, Id, EdgeProp>> result;
        std::unordered_set<std::string> seen;
        for (const auto &[from, vec] : adj_) {
            for (const auto &[to, prop] : vec) {
                if (!directed_ && from > to) continue;
                std::ostringstream key;
                key << from << "-" << to;
                if (seen.count(key.str())) continue;
                seen.insert(key.str());
                result.emplace_back(from, to, prop);
            }
        }
        return result;
    }


private:
    bool remove_edge_internal(const id_type& from, const id_type& to) {
        auto it = adj_.find(from);
        if (it == adj_.end()) return false;
        auto &vec = it->second;
        auto old_sz = vec.size();
        vec.erase(std::remove_if(vec.begin(), vec.end(),
                                 [&](const Edge& e){ return e.first == to; }),
                  vec.end());
        return vec.size() != old_sz;
    }

    bool directed_;
    AdjacencyList adj_;
    std::unordered_map<id_type, node_ptr> nodes_;
};

#endif // GRAPH_HPP
