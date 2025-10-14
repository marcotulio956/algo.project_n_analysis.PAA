#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = (1LL<<60);

int main(){
    int n, m;
    int instance = 1;
    while ( (cin >> n >> m) ) {
        vector<vector<ll>> base(n+1, vector<ll>(n+1, INF));
        // next0[k][i][j] will store the next hop from i to j when using intermediates up to k
        vector<vector<vector<int>>> nextIdx(
            n+1,
            vector<vector<int>>(
                n+1,
                vector<int>(n+1, -1)
            )
        );
        for (int i = 1; i <= n; ++i) base[i][i] = 0;
        for (int i = 0; i < m; ++i) {
            int u, v, w;
            cin >> u >> v >> w;
            if (w < base[u][v]) base[u][v] = w; // keep smallest if multiple edges provided
        }

        // store snapshots after allowing intermediates 1 to k for k=1 to n
        // dp[k][i][j] = shortest i->j using only intermediates in 1 to k
        // arround (n+1)^3 ~ 1e6 entries when n=100 -> OK
        vector<vector<vector<ll>>> dp(
            n+1,
            vector<vector<ll>>(
                n+1,
                vector<ll>(n+1, INF)
            )
        );

        // dp[0] = base distances // initialize nextIdx[0]
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j) {
                dp[0][i][j] = base[i][j];
                if (base[i][j] < INF) nextIdx[0][i][j] = j; // next hop from i to j is j itself
            }
            // self loops
            dp[0][i][i] = 0;
            nextIdx[0][i][i] = i;
        }


        // saving dp[k] and nextIdx[k] for each k
        for (int k = 1; k <= n; ++k) {
            // copy past 
            for (int i = 1; i <= n; ++i) {
                for (int j = 1; j <= n; ++j) {
                    dp[k][i][j] = dp[k-1][i][j];
                    nextIdx[k][i][j] = nextIdx[k-1][i][j];
                }
            }

            // try allowing k as intermediate
            for (int i = 1; i <= n; ++i) {
                if (dp[k-1][i][k] == INF) continue;
                for (int j = 1; j <= n; ++j) {
                    if (dp[k-1][k][j] == INF) continue;
                    ll cand = dp[k-1][i][k] + dp[k-1][k][j];
                    if (cand < dp[k][i][j]) {
                        dp[k][i][j] = cand;
                        // path from i to j now goes i -> (next hop on i->k using k-1)
                        nextIdx[k][i][j] = nextIdx[k-1][i][k];
                    }
                }
            }
        }


        int c; cin >> c;
        cout << "Instancia " << instance++ << "\n";

        for (int qi = 0; qi < c; ++qi) {
            int o, d, t;
            cin >> o >> d >> t;
            if (t < 0) t = 0;
            if (t > n) t = n;
            ll res = dp[t][o][d];
            if (res >= INF) {
                cout << -1 << "\n";
            } else {
                cout << res << "\n";
                // reconstruct path using nextIdx[t]
                vector<int> path;
                int cur = o;
                path.push_back(cur);
                int safety = 0;
                while (cur != d && safety <= n+5) {
                    int nxt = nextIdx[t][cur][d];
                    if (nxt == -1) break; // shouldn't happen
                    path.push_back(nxt);
                    cur = nxt;
                    ++safety;
                }
                // for (size_t i = 0; i < path.size(); ++i) {
                //     if (i) cout << ' ';
                //     cout << path[i];
                // }
                // cout << "\n";
            }
        }


        cout << "\n";
    }

    return 0;
}
