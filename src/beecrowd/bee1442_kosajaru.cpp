#include <bits/stdc++.h>
using namespace std;

using vetint = vector<int>;
using vetch = vector<char>;
using adjtype = vector<vector<int>>;

struct Edge { int u, v, t; };

struct PairHash {
    size_t operator()(const pair<int,int>& p) const noexcept {
        return ((uint64_t)p.first << 32) ^ (uint64_t)p.second;
    }
};

void dfs_stack(int s, adjtype& g, vetch& vis) {
    stack<int> st;
    st.push(s);
    vis[s] = 1;
    while (!st.empty()) {
        int u = st.top(); st.pop();
        for (int v : g[u]) if (!vis[v]) { vis[v] = 1; st.push(v); }
    }
}

void kosaraju_dfs1(int u, adjtype& g, vetch& vis, vetint& order) {
    vis[u] = 1;
    for (int v : g[u])
        if (!vis[v])
            kosaraju_dfs1(v, g, vis, order);
    order.push_back(u);
}

void kosaraju_dfs2(int u, adjtype& grev, vetch& vis, vetint& comp, int cid) {
    vis[u] = 1;
    comp[u] = cid;
    for (int v : grev[u])
        if (!vis[v])
            kosaraju_dfs2(v, grev, vis, comp, cid);
}


void dfs_br_func(int u, int parent_eid,
                 vetch& visb,
                 vetint& tin,
                 vetint& low,
                 int &timer,
                 const vector<vector<pair<int,int>>>& cond_adj,
                 const vector<pair<int,int>>& edges_cond,
                 const unordered_map<pair<int,int>,int,PairHash>& sccs,
                 bool &must_convert)
{
    visb[u] = 1;
    tin[u] = low[u] = ++timer;

    for (const auto &p : cond_adj[u]) {
        int v = p.first;
        int id = p.second;

        if (id == parent_eid) continue;

        if (visb[v]) {
            
            low[u] = min(low[u], tin[v]);
        } else {
            dfs_br_func(v, id, visb, tin, low, timer, cond_adj, edges_cond, sccs, must_convert);
            low[u] = min(low[u], low[v]);
            if (low[v] > tin[u]) {
                
                auto pr = edges_cond[id];
                int a = pr.first, b = pr.second;
                int mult = sccs.at({a,b}); 
                if (mult == 1) must_convert = true;
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    while (true) {
        int N, M;
        if (!(cin >> N >> M)) return 0;

        vector<Edge> edges;
        edges.reserve(M);
        for (int i = 0; i < M; ++i) {
            int A, B, T; cin >> A >> B >> T;
            --A; --B; 
            edges.push_back({A, B, T});
        }

        if (M == 0) { cout << "*\n"; continue; }

        
        adjtype adj(N), rev(N);
        adjtype undirected(N);
        for (int i = 1; i < M; ++i) {
            int u = edges[i].u, v = edges[i].v, t = edges[i].t;
            if (t == 1) {
                
                adj[u].push_back(v);
                rev[v].push_back(u);
            } else {
                
                adj[u].push_back(v);
                adj[v].push_back(u);
                rev[u].push_back(v);
                rev[v].push_back(u);
            }
            
            undirected[u].push_back(v);
            undirected[v].push_back(u);
        }

        
        vetch vis(N, 0);
        dfs_stack(0, undirected, vis);
        bool und_conn = true;
        for (int i = 0; i < N; ++i) if (!vis[i]) { und_conn = false; break; }
        if (!und_conn) { cout << "*\n"; continue; }

        
        vetch used(N, 0);
        vetint order; order.reserve(N);
        for (int i = 0; i < N; ++i) if (!used[i]) kosaraju_dfs1(i, adj, used, order);

        fill(used.begin(), used.end(), 0);
        vetint comp(N, -1);
        int cid = 0;
        for (int i = (int)order.size() - 1; i >= 0; --i) {
            int u = order[i];
            if (!used[u]) kosaraju_dfs2(u, rev, used, comp, cid++);
        }

        if (cid == 1) { cout << "-\n"; continue; } 

        
        unordered_map<pair<int,int>, int, PairHash> sccs;
        for (int i = 1; i < M; ++i) {
            int u = edges[i].u, v = edges[i].v;
            int cu = comp[u], cv = comp[v];
            if (cu == -1 || cv == -1) continue;
            if (cu == cv) continue;
            int a = min(cu, cv), b = max(cu, cv);
            sccs[{a, b}]++;
        }

        
        int K = cid;
        vector<vector<pair<int,int>>> cond_adj(K); 
        vector<pair<int,int>> edges_cond;
        edges_cond.reserve(sccs.size());
        int eid = 0;
        for (auto &kv : sccs) {
            auto p = kv.first;
            int a = p.first, b = p.second;
            cond_adj[a].push_back({b, eid});
            cond_adj[b].push_back({a, eid});
            edges_cond.push_back({a, b});
            ++eid;
        }

        
        vetch visc(K, 0);
        stack<int> st; st.push(0); visc[0] = 1;
        while (!st.empty()) {
            int u = st.top(); st.pop();
            for (auto [v, _] : cond_adj[u]) if (!visc[v]) { visc[v] = 1; st.push(v); }
        }
        bool cond_connected = true;
        for (int i = 0; i < K; ++i) if (!visc[i]) { cond_connected = false; break; }
        if (!cond_connected) { cout << "*\n"; continue; }

        
        vetint tin(K, -1), low(K, -1);
        vetch visb(K, 0);
        int timer = 0;
        bool must_convert = false;

        dfs_br_func(0, -1, visb, tin, low, timer, cond_adj, edges_cond, sccs, must_convert);

        if (must_convert) cout << "2\n"; else cout << "1\n";
    }

    return 0;
}
