#include <bits/stdc++.h>
using namespace std;

#define MAXV 500
#define ADJ 1
#define NADJ 0
#define WHITE 0
#define CINZA 1
#define PRETO 2
#define NULO -1
#define MAX_AGE 101

static char m[MAXV + 1][MAXV + 1];
static int N;
static int c[MAXV + 1];
static int d[MAXV + 1];
static int t[MAXV + 1];
static int a[MAXV + 1];
static int finishTime;
static int ages[MAXV + 1];
static int age;

void printvet(int vet[]){
    for(int i = 1; i <= N; i++){
        cout << vet[i] << ", ";
    }
    cout << "\n";
}

void init(){
    memset(m, NADJ, sizeof(m));
}

void DFS_VISIT(int u){
    c[u] = CINZA;
    finishTime++;
    d[u] = finishTime;
    for(int v = 1; v <= N; v++){
        if(m[u][v] == ADJ){
            if(c[v] == WHITE){
                if (ages[v] < age) age = ages[v];
                a[v] = u;
                DFS_VISIT(v);
            }
        }
    }
    c[u] = PRETO;
    finishTime++;
    t[u] = finishTime;
}

void DFS(int emp){
    age = MAX_AGE;
    for(int u = 1; u <= N; u++){
        c[u] = WHITE;
        d[u] = NULO;
        t[u] = NULO;
        a[u] = NULO;
    }
    finishTime = 0;
    if(c[emp] == WHITE) DFS_VISIT(emp);
}



void swap(int mng, int empl) {
    int aux;
    for (int i = 1; i <= N; i++) {
        aux = m[mng][i];
        m[mng][i] = m[empl][i];
        m[empl][i] = aux;
    }
    for (int i = 1; i <= N; i++) {
        aux = m[i][mng];
        m[i][mng] = m[i][empl];
        m[i][empl] = aux;
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int np, nr, ni;
    while (cin >> np >> nr >> ni){
        init();
        N = np;
        for (int i = 1; i <= np; i++){
            cin >> ages[i];
        }
        int mng, empl;
        for (int i = 1; i <= nr; i++){
            cin >> mng >> empl;
            m[empl][mng] = ADJ;
        }
        char isntr;
        for (int i = 1; i <= ni; i++){
            cin >> isntr;
            if (isntr == 'P'){
                int emp;
                cin >> emp;
                DFS(emp);
                if (age == MAX_AGE) cout << "*\n";
                else cout << age << "\n";
            }
            if (isntr == 'T'){
                cin >> mng >> empl;
                swap(mng, empl);
            }
        }
    }
    return 0;
}
