#include <iostream>
#include <vector>
#include <stack>

using namespace std;
using adj_type = vector<vector<int>>;

bool dfs_check_cycle(adj_type& adj, int& starting_document, vector<int>& visited) {
    stack<pair<int, bool>> tovisit; 
    tovisit.push({starting_document, false}); 
    while (!tovisit.empty()) {
        auto [visiting, post_visit] = tovisit.top(); tovisit.pop();
        if (post_visit) {
            visited[visiting] = 2;
            continue;
        }
        if (visited[visiting] == 1) {
            return true;
        }
        if (visited[visiting] == 0) {
            visited[visiting] = 1;
            tovisit.push({visiting, true});
            for (int neighbor : adj[visiting]) {
                if (visited[neighbor] != 2) { 
                    tovisit.push({neighbor, false});
                }
            }
        }
    }

    return false; 
}

int main() {
    int T, N, M;
    cin >> T;

    for (size_t i = 0; i < T; i++) {
        cin >> N >> M;
        adj_type adj(N, vector<int>(0, 0)); 
        vector<int> visited(N, 0); 

        
        for (size_t j = 0; j < M; j++) {
            int A, B;
            cin >> A >> B;
            adj[A - 1].push_back(B - 1);
        }

        bool cycle = false;

        
        for (size_t i = 0; i < N && !cycle; i++) {
            if (visited[i] == 0) { 
                int startVisiting = i;
                if (dfs_check_cycle(adj, startVisiting, visited)) {
                    cycle = true; 
                }
            }
        }

        
        if (cycle)
            cout << "SIM" << endl; 
        else
            cout << "NAO" << endl; 
    }

    return 0;
}
