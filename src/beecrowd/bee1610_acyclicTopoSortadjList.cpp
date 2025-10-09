#include <iostream>
#include <vector>
#include <queue>

using namespace std;
using adj_type = vector<vector<int>>;

int main() {
    int T, N, M;
    cin >> T;

    for (size_t i = 0; i < T; i++) {
        cin >> N >> M;
        adj_type adj(N, vector<int>(0, 0)); 
        vector<int> visited(N, 0); 
        vector<int> indeg(N, 0);

        
        for (size_t j = 0; j < M; j++) {
            int A, B;
            cin >> A >> B;
            adj[A - 1].push_back(B - 1);
            indeg[B - 1] += 1;
        }

        queue<int> q;

        bool cycle = false;

        for(int i=0; i<indeg.size(); i++){
            if(indeg[i] == 0) { 
                q.push(i); // pushes the node which has no incoming edges
            }
        }

        vector<int> result;

        while(!q.empty()) {
            int zeroIndegNode = q.front(); q.pop();
            result.push_back(zeroIndegNode);

            for(int j=0; j<adj[zeroIndegNode].size(); j++) { 
                int edgeTo = adj[zeroIndegNode][j];
                if(--indeg[edgeTo]==0) {
                    q.push(edgeTo);
                }
            }
        }

        cycle = result.size() == N ? false : true;
                
        if (cycle)
            cout << "SIM" << endl; 
        else
            cout << "NAO" << endl; 
    }

    return 0;
}
