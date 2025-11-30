#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main()
{
    int N;
    while (cin >> N)
    {
        if (!N)
            return 0;

        vector<pair<int, int>> t(N);
        for (int i = 0; i < N; i++)
            cin >> t[i].first >> t[i].second;

        // ordena por tempo free
        sort(t.begin(), t.end());

        ll cur = 1; // tempo atual
        int i = 0;

        while (i < N)
        {
            if (t[i].first > cur)
            {
                cur = t[i].first;
            }

            cur += t[i].second;
            i++;
        }

        cout << cur << "\n";
    }
}
