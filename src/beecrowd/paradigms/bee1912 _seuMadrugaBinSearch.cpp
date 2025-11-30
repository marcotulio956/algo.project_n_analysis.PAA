#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main()
{
    int N;
    ll A;
    while ((cin >> N >> A))
    {
        if (N == 0 && A == 0)
            return 0;

        vector<ll> C(N);
        ll total = 0;
        ll mx = 0;
        for (int i = 0; i < N; ++i)
        {
            cin >> C[i];
            total += C[i];
            mx = max(mx, C[i]);
        }

        if (A == 0)
        {
            cout << ":D\n";
            continue;
        }
        if (total < A)
        {
            cout << "-.-\n";
            continue;
        }

        sort(C.begin(), C.end(), greater<ll>());

        vector<double> pref(N + 1, 0);
        for (int i = 0; i < N; ++i)
            pref[i + 1] = pref[i] + C[i];

        bool found = false;
        double H = 0;

        for (int k = 1; k <= N; ++k)
        {
            double Sk = pref[k];      // sum of top k lengths
            double hk = (Sk - A) / k; // candidate H

            // in 0-based: C[k-1] >= H > C[k]  (and define C[N] = 0)
            double upper = C[k - 1];              // C_k
            double lower = (k < N) ? C[k] : 0.0L; // C_{k+1} or 0

            // IA helped:
            // allow small eps tolerance for float comparisons
            const long double EPS = 1e-12L;

            if (hk <= upper + EPS && hk > lower - EPS)
            {
                H = hk;
                if (H < 0)
                    H = 0;
                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "-.-\n";
        }
        else
        {
            if (H == 0)
                cout << ":D\n";
            else
                cout << fixed << setprecision(4) << H << "\n";
        }
    }

    return 0;
}