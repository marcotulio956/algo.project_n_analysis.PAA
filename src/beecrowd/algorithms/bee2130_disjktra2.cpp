#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = (1LL<<60);

int main() {
    int n, m;
    int instance = 1;
    while ( (cin >> n >> m) ) {
        vector<vector<pair<int,int>>> adj(n+1);
        for (int i = 0; i < m; ++i) {
            int u,v,w; cin >> u >> v >> w;
            adj[u].push_back({v,w});
        }

        int c; cin >> c;
        struct Query { int o,d,t,idx; };
        vector<Query> queries;
        queries.reserve(c);
        for (int i = 0; i < c; ++i) {
            int o,d,t; cin >> o >> d >> t;
            queries.push_back({o,d,t,i});
        }

        vector<ll> ans(c, -1);
        unordered_map<int, vector<Query>> by_t;
        by_t.reserve(128);
        for (auto &q : queries) by_t[q.t].push_back(q);

        for (auto &kv : by_t) {
            int t = kv.first;
            auto &vec = kv.second;

            unordered_map<int, vector<Query>> by_origin;
            by_origin.reserve(128);
            for (auto &q : vec) by_origin[q.o].push_back(q);

            // for each origin in this t
            for (auto &kv2 : by_origin) {
                int origin = kv2.first;
                auto &vqueries = kv2.second;

                // multiple queries can ask same o, d, t
                unordered_map<int, vector<int>> dest_to_qidx;
                dest_to_qidx.reserve(vqueries.size()*2);
                int remaining = 0;
                for (auto &q : vqueries) {
                    if (q.o == q.d) {
                        ans[q.idx] = 0; // trivial
                    } else {
                        dest_to_qidx[q.d].push_back(q.idx);
                        // count unique destinations only
                    }
                }

                // number of unique destinations
                remaining = (int)dest_to_qidx.size();
                if (remaining == 0) continue; // nothing to do (all queries were o==d)

                vector<ll> dist(n+1, INF);
                vector<char> seen(n+1, 0);
                priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
                dist[origin] = 0;
                pq.push({0, origin});

                while (!pq.empty() && remaining > 0) {
                    auto [du, u] = pq.top(); pq.pop();
                    if (du != dist[u]) continue;
                    auto it = dest_to_qidx.find(u);
                    if (it != dest_to_qidx.end()) {
                        // set all query answers that ask for this destination
                        for (int qidx : it->second) ans[qidx] = du;
                        // remove entry and decrement remaining
                        dest_to_qidx.erase(it);
                        --remaining;
                        if (remaining == 0) break;
                    }

                    // relax neighbors:
                    // relaxing if u == origin 
                    // OR u <= t (some stopover)
                    if (u != origin && u > t) {
                        // u is a sink (can't continue from this node), so skip relaxations
                        continue;
                    }

                    for (auto &e : adj[u]) {
                        int v = e.first;
                        int w = e.second;
                        if (dist[v] > du + w) {
                            dist[v] = du + w;
                            pq.push({dist[v], v});
                        }
                    }
                }
            }
        }
        cout << "Instancia " << instance++ << "\n";
        for (int i = 0; i < c; ++i) {
            cout << (ans[i] == -1 ? -1 : ans[i]) << "\n";
        }
        cout << "\n";
    }

    return 0;
}
