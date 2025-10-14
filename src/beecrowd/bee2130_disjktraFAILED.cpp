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
            int u, v, w;
            cin >> u >> v >> w;
            adj[u].push_back({v, w}); // directed edge u -> v
        }

        int c;
        cin >> c;
        cout << "Instance " << instance++ << "\n";

        for (int qi = 0; qi < c; ++qi) {
            int o, d, t;
            cin >> o >> d >> t;

            if (o == d) {
                cout << 0 << "\n";
                continue;
            }
            vector<ll> dist(n+1, INF);
            priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
            dist[o] = 0;
            pq.push({0, o});
            ll answer = -1;

            while (!pq.empty()) {
                auto [du, u] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                if (u == d) { answer = du; break; } // early exit
                for (auto &e : adj[u]) {
                    int v = e.first;
                    int w = e.second;
                    // allow v only if:
                    // - v is in allowed stopovers 1..t, OR
                    // - v is the destination
                    // origin is already the start, we don't need special-case here for v==o
                    if ( (v <= t) || (v == d) ) {
                        if (dist[v] > du + w) {
                            dist[v] = du + w;
                            pq.push({dist[v], v});
                        }
                    }
                }
            }
            if (answer == -1) cout << -1 << "\n";
            else cout << answer << "\n";
        }
        cout << "\n"; // end of instance
    }
    return 0;
}
