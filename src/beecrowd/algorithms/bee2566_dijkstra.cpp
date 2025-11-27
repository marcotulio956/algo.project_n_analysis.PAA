#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = (1LL<<60);

int main() {
    while (true) {
        int N, M;
        if (!(cin >> N >> M)) return 0;

        vector<vector<pair<int,int>>> adjBus(N), adjPlane(N);

        for (int i = 0; i < M; ++i) {
            int A, B, T, R;
            cin >> A >> B >> T >> R;
            --A; --B;
            if (T == 0) {
                adjBus[A].push_back({B, R});
            } else {
                adjPlane[A].push_back({B, R});
            }
        }

        auto dijkstra = [&](const vector<vector<pair<int,int>>>& g) -> ll {
            vector<ll> dist(N, INF);
            priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;
            dist[0] = 0;
            pq.push({0, 0});
            while (!pq.empty()) {
                auto [d, u] = pq.top(); pq.pop();
                if (d != dist[u]) continue;
                if (u == N-1) return d; // early exit when reaching target
                for (auto &e : g[u]) {
                    int v = e.first;
                    int w = e.second;
                    if (dist[v] > d + w) {
                        dist[v] = d + w;
                        pq.push({dist[v], v});
                    }
                }
            }
            return dist[N-1];
        };

        ll costBus = dijkstra(adjBus);
        ll costPlane = dijkstra(adjPlane);
        ll ans = min(costBus, costPlane);

        cout << ans << "\n";
    }
    return 0;
}
