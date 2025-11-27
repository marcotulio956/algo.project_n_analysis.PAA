#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int u, v;
    int w;
    Edge() {}
    Edge(int _u, int _v, int _w) : u(_u), v(_v), w(_w) {}
};

struct DSU {
    vector<int> p, r;

    DSU(int n = 0) {
        init(n);
    }

    void init(int n) {
        p.resize(n);
        r.assign(n, 0);
        for (int i = 0; i < n; ++i) p[i] = i;
    }
    
    int find(int x) {
        if (p[x] == x) return x;
        return p[x] = find(p[x]);
    }

    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) ++r[a];
        return true;
    }
};

int main() {
    int N, M;
    while ( (cin >> N >> M) ) {
        vector<Edge> edges;
        edges.reserve(M);

        for (int i = 0; i < M; ++i) {
            int A, B, C;
            cin >> A >> B >> C;
            --A; --B; // zero-based index
            edges.emplace_back(A, B, C);
        }

        // If there are not enough edges to connect all nodes -> impossible
        if (M < N - 1) {
            cout << "impossivel\n";
            continue;
        }

        sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) { // n log n on avg input (quick sort)
            return a.w < b.w;
        });

        DSU dsu(N);
        long long total = 0;
        int taken = 0;

        for (const auto &e : edges) {
            if (dsu.unite(e.u, e.v)) {
                total += e.w;
                ++taken;
                if (taken == N - 1) break;
            }
        }

        if (taken == N - 1) cout << total << "\n";
        else                 cout << "impossivel\n";

    }

    return 0;
}
