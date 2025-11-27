#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
  int N;
  while ((cin >> N)) {
    vector<ll> a(N);
    for (int i = 0; i < N; ++i)
      cin >> a[i];

    vector<ll> pref(N + 1, 0);
    for (int i = 0; i < N; ++i)
      pref[i + 1] = pref[i] + a[i];
    auto range_sum = [&](int l, int r) -> ll { return pref[r + 1] - pref[l]; };

    // dp[i] store F(i, j) for ite
    // init length=1: F(i,i) = a[i]
    vector<ll> dp(N);
    for (int i = 0; i < N; ++i)
      dp[i] = a[i];

    for (int len = 2; len <= N; ++len) {
      for (int i = 0; i + len - 1 < N; ++i) {
        int j = i + len - 1;
        ll s = range_sum(i, j);
        // dp[i]   holds F(i, j-1)
        // dp[i+1] holds F(i+1, j)
        // new F(i,j) -stores-> dp[i]
        ll takeLeft = s - dp[i + 1];
        ll takeRight = s - dp[i];
        dp[i] = max(takeLeft, takeRight);
      }
    }
    cout << dp[0] << '\n';
  }
  return 0;
}
