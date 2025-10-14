#include <bits/stdc++.h>
using namespace std;
using adj_type = vector<vector<int>>;

bool dfs_check_cycle(adj_type& adj, int starting_document, vector<int>& visited, vector<vector<int>>& trees, vector<int>& start_time, vector<int>& finish_time, int& time_counter) {
    stack<pair<int, bool>> tovisit;
    tovisit.push({starting_document, false});
    vector<int> current_tree; // To store the current DFS tree

    while (!tovisit.empty()) {
        auto [visiting, post_visit] = tovisit.top();
        tovisit.pop();

        if (post_visit) {
            // Finish time: we have fully explored this node
            finish_time[visiting] = ++time_counter;
            visited[visiting] = 2;
            continue;
        }

        if (visited[visiting] == 1) {
            return true; // Cycle detected
        }

        if (visited[visiting] == 0) {
            // Start time: we're starting to explore this node
            start_time[visiting] = ++time_counter;
            visited[visiting] = 1;
            current_tree.push_back(visiting); // Add to the current tree
            tovisit.push({visiting, true});

            for (int neighbor : adj[visiting]) {
                if (visited[neighbor] != 2) {
                    tovisit.push({neighbor, false});
                }
            }
        }
    }

    if (!current_tree.empty()) {
        trees.push_back(current_tree); // Store the DFS tree
    }

    return false;
}

int main() {
    int T, N, M;
    cin >> T;

    for (size_t t = 0; t < T; t++) {
        cin >> N >> M;
        adj_type adj(N);
        vector<int> visited(N, 0);
        vector<vector<int>> trees; // To store all trees found
        vector<int> start_time(N, -1); // Start times of each node
        vector<int> finish_time(N, -1); // Finish times of each node
        int time_counter = 0; // Global time counter

        // Input the edges and build the adjacency list
        cout << "Adjacency List for Test Case " << t + 1 << ":" << endl;
        for (size_t j = 0; j < M; j++) {
            int A, B;
            cin >> A >> B;
            adj[A - 1].push_back(B - 1);
        }

        // Output the adjacency list
        for (int i = 0; i < N; ++i) {
            cout << i + 1 << ": ";
            for (int neighbor : adj[i]) {
                cout << neighbor + 1 << " ";
            }
            cout << endl;
        }

        bool cycle = false;

        // Perform DFS for each unvisited node and detect cycle
        for (size_t i = 0; i < N && !cycle; i++) {
            if (visited[i] == 0) {
                int startVisiting = i;
                if (dfs_check_cycle(adj, startVisiting, visited, trees, start_time, finish_time, time_counter)) {
                    cycle = true; // Cycle detected
                }
            }
        }

        // Output the result (cycle detection)
        if (cycle) {
            cout << "SIM" << endl;
        } else {
            cout << "NAO" << endl;
        }

        // Output the DFS trees
        cout << "DFS Trees:" << endl;
        for (const auto& tree : trees) {
            for (int node : tree) {
                cout << node + 1 << " ";
            }
            cout << endl;
        }

        // Output start and finish times of each node
        cout << "Start and Finish Times:" << endl;
        for (int i = 0; i < N; ++i) {
            cout << "Node " << i + 1 << " - Start: " << start_time[i] << ", Finish: " << finish_time[i] << endl;
        }

    }

    return 0;
}
