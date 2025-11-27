#include <bits/stdc++.h>
using namespace std;

void shift_or(int shift, int W, vector<unsigned long long> &dp_words) {
  if (shift == 0) {
    // dp |= dp (sem alteração)
    return;
  }
  int whole = shift / 64;
  int rem = shift % 64;
  vector<unsigned long long> shifted(W, 0ULL);

  for (int i = 0; i < W; ++i) {
    unsigned long long val = dp_words[i];
    if (val == 0)
      continue;
    int j = i + whole;
    if (j < W) {
      if (rem == 0) {
        shifted[j] |= val;
      } else {
        shifted[j] |= (val << rem);
        if (j + 1 < W) {
          shifted[j + 1] |= (val >> (64 - rem));
        }
      }
    } else {
      continue;
    }
  }
  for (int i = 0; i < W; ++i)
    dp_words[i] |= shifted[i];
}

int main() {
  int R, K;
  while ((cin >> R >> K)) {
    vector<int> deg(R + 1, 0);
    for (int i = 0; i < K; ++i) {
      int a, b;
      cin >> a >> b;
      deg[a]++;
      deg[b]++;
    }

    int target = K;
    vector<unsigned long long> dp_words; // bits
    int W = (target + 64) / 64;          // número de palavras de 64 bits
    dp_words.assign(W, 0ULL);            // unsigned long zero
    dp_words[0] = 1ULL;                  // soma 0 possível

    for (int v = 1; v <= R; ++v) {
      int d = deg[v];
      if (d <= 0)
        continue;
      if (d > target)
        d = d;
      shift_or(d, W, dp_words);
    }

    int word = target / 64;
    int bit = target % 64;
    bool possible = false;
    if (word < W) {
      possible = ((dp_words[word] >> bit) & 1ULL);
    } else
      possible = false;

    cout << (possible ? 'S' : 'N') << '\n';
  }

  return 0;
}
