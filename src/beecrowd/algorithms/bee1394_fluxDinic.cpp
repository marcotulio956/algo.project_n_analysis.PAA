#include <bits/stdc++.h>
using namespace std;

int N, M, G;
vector<vector<int>> adj; 
vector<int> scores;     


void applyResult(int t1, char ch, int t2) {
    if (ch == '<') {
        scores[t2] += 2;
    } else if (ch == '=') {
        scores[t1] += 1;
        scores[t2] += 1;
    } else {
        scores[t1] += 2;
    }
    if (adj[t1][t2] > 0) {
        adj[t1][t2] -= 1;
        adj[t2][t1] -= 1;
    }
}


struct Edge {
    int to, cap, rev;
    Edge(int _to, int _cap, int _rev) : to(_to), cap(_cap), rev(_rev) {}
};

struct Dinic {
    int n;
    vector<vector<Edge>> g;
    vector<int> level, it;
    Dinic(int n=0): n(n), g(n), level(n), it(n) {}

    void init(int _n) {
        n = _n;
        g.assign(n, {});
        level.assign(n, 0);
        it.assign(n, 0);
    }

    void addEdge(int u, int v, int cap) {
        g[u].emplace_back(v, cap, (int)g[v].size());
        g[v].emplace_back(u, 0, (int)g[u].size()-1);
    }

    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while(!q.empty()) {
            int u = q.front(); q.pop();
            for (const Edge &e : g[u]) if (e.cap > 0 && level[e.to] < 0) {
                level[e.to] = level[u] + 1;
                q.push(e.to);
            }
        }
        return level[t] >= 0;
    }

    int dfs(int u, int t, int f) {
        if (u == t) return f;
        for (int &i = it[u]; i < (int)g[u].size(); ++i) {
            Edge &e = g[u][i];
            if (e.cap > 0 && level[e.to] == level[u] + 1) {
                int pushed = dfs(e.to, t, min(f, e.cap));
                if (pushed > 0) {
                    e.cap -= pushed;
                    g[e.to][e.rev].cap += pushed;
                    return pushed;
                }
            }
        }
        return 0;
    }

    long long maxflow(int s, int t) {
        long long flow = 0;
        const int INF = numeric_limits<int>::max();
        while (bfs(s,t)) {
            fill(it.begin(), it.end(), 0);
            while (true) {
                int pushed = dfs(s,t,INF);
                if (pushed == 0) break;
                flow += pushed;
            }
        }
        return flow;
    }
};


bool canDreamWin() {
    vector<int> pts = scores;
    vector<vector<int>> rem = adj; 
    int rem0_sum = 0;
    for (int j = 1; j < N; ++j) {
        rem0_sum += rem[0][j];
        rem[0][j] = rem[j][0] = 0;
    }
    pts[0] += 2 * rem0_sum;
    int target = pts[0]; 
    vector<int> cap(N, 0);
    for (int i = 1; i < N; ++i) {
        if (pts[i] >= target) return false; 
        cap[i] = (target - 1) - pts[i];
    }
    vector<pair<int,int>> games;
    long long totalPoints = 0;
    for (int i = 1; i < N; ++i) {
        for (int j = i+1; j < N; ++j) {
            if (rem[i][j] > 0) {
                games.emplace_back(i, j);
                totalPoints += 2 * rem[i][j];
            }
        }
    }

    long long sumCaps = 0;
    for (int i = 1; i < N; ++i) sumCaps += max(0, cap[i]);
    if (sumCaps < totalPoints) return false;

    
    int Gnodes = (int)games.size();
    int source = 0;
    int gameStart = 1;
    int teamStart = gameStart + Gnodes; 
    int sink = teamStart + (N-1);       
    int nodesCount = sink + 1;

    Dinic dinic(nodesCount);

    for (int idx = 0; idx < Gnodes; ++idx) {
        int i = games[idx].first;
        int j = games[idx].second;
        int remMatches = rem[i][j];
        int ptsAvail = 2 * remMatches;
        int gameNode = gameStart + idx;
        dinic.addEdge(source, gameNode, ptsAvail);
        dinic.addEdge(gameNode, teamStart + (i-1), ptsAvail);
        dinic.addEdge(gameNode, teamStart + (j-1), ptsAvail);
    }

    for (int i = 1; i < N; ++i) {
        int teamNode = teamStart + (i-1);
        int c = cap[i];
        if (c < 0) return false; 
        dinic.addEdge(teamNode, sink, c);
    }

    long long flow = dinic.maxflow(source, sink);
    return flow == totalPoints;
}

int main() {
    while (cin >> N >> M >> G) {
        if (N == 0 && M == 0 && G == 0) break;

        
        adj.assign(N, vector<int>(N, 0));
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                adj[i][j] = (i == j) ? 0 : M;
            }
        }
        scores.assign(N, 0);

        
        for (int k = 0; k < G; ++k) {
            int t1, t2;
            char ch;
            cin >> t1 >> ch >> t2;
            applyResult(t1, ch, t2);
        }

        
        bool ok = canDreamWin();
        cout << (ok ? 'Y' : 'N') << "\n";
    }

    return 0;
}
