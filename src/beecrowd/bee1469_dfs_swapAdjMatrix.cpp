#include <bits/stdc++.h>
using namespace std;

vector<vector<unsigned char>> adj;
vector<int> ages;
int numEmployees;

void swapPositions(int a, int b){
    if (a == b){
        return;
    }

    for (int col = 1; col <= numEmployees; ++col){
        unsigned char temp = adj[a][col];
        adj[a][col] = adj[b][col];
        adj[b][col] = temp;
    }

    for (int row = 1; row <= numEmployees; ++row){
        unsigned char temp = adj[row][a];
        adj[row][a] = adj[row][b];
        adj[row][b] = temp;
    }
}

void dfs(int u, vector<int> &color, int &bestAge){
    color[u] = 1;

    for (int v = 1; v <= numEmployees; ++v){
        if (adj[u][v] == 1){
            if (color[v] == 0){
                if (ages[v] < bestAge){
                    bestAge = ages[v];
                }
                dfs(v, color, bestAge);
            }
        }
    }

    color[u] = 2;
}

void queryYoungestManager(int employee){
    vector<int> color(numEmployees + 1, 0);
    int bestAge = 101;

    dfs(employee, color, bestAge);

    if (bestAge == 101){
        cout << "*\n";
    }
    else{
        cout << bestAge << "\n";
    }
}

int main(){

    while (true){
        int numRelations;
        int numInstructions;

        if (!(cin >> numEmployees >> numRelations >> numInstructions)){
            break;
        }

        ages.assign(numEmployees + 1, 0);

        for (int i = 1; i <= numEmployees; ++i){
            cin >> ages[i];
        }

        adj.assign(numEmployees + 1, vector<unsigned char>(numEmployees + 1, 0));

        for (int i = 0; i < numRelations; ++i){
            int manager;
            int subordinate;
            cin >> manager >> subordinate;
            adj[subordinate][manager] = 1;
        }

        for (int i = 0; i < numInstructions; ++i){
            char instruction;
            cin >> instruction;

            if (instruction == 'T'){
                int a;
                int b;
                cin >> a >> b;
                swapPositions(a, b);
            }
            else if (instruction == 'P'){
                int e;
                cin >> e;
                queryYoungestManager(e);
            }
        }
    }
    return 0;
}
