#include <bits/stdc++.h>

using ull = unsigned long long;
const ull MOD = 1000000ULL;
using namespace std;

void printm(std::vector<int> m)
{ // print m in reverse
    for (int i = m.size() - 1; i >= 0; i--)
    {
        std::cout << m[i];
    }
    std::cout << std::endl;
}

void printmtx(std::vector<std::vector<int>> dp)
{
    for (int i = 0; i < int(dp.size()); i++)
    {
        for (int j = 0; j < int(dp[i].size()); j++)
        {
            std::cout << dp[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

struct Mat
{
    ull a00, a01, a10, a11;
    Mat(ull w00 = 0, ull w01 = 0, ull w10 = 0, ull w11 = 0) : a00(w00), a01(w01), a10(w10), a11(w11) {}
};

Mat mul(const Mat &A, const Mat &B)
{
    Mat C;
    C.a00 = ((A.a10 * B.a01) + (A.a01 * B.a10)) % MOD;
    C.a01 = ((A.a10 * B.a00) + (A.a01 * B.a11)) % MOD;
    C.a10 = ((A.a00 * B.a01) + (A.a11 * B.a10)) % MOD;
    C.a11 = ((A.a00 * B.a00) + (A.a11 * B.a11)) % MOD;
    return C;
}

Mat matPow(Mat base, unsigned long long exp)
{
    Mat res(1, 0, 0, 1); // eye 2
    while (exp)
    {
        if (exp & 1ULL)
            res = mul(res, base);
        base = mul(base, base);
        exp >>= 1ULL;
    }
    return res;
}

int main()
{
    ull N, K, L;
    while ((cin >> N >> K >> L))
    {
        ull M = N / 5ULL; // guaranteed N multiple of 5

        ull k = K % MOD;
        ull l = L % MOD;

        if (M == 0ULL)
        {
            cout << "000000\n";
            continue;
        }
        if (M == 1ULL)
        {
            ull ans = k % MOD;
            cout << setw(6) << setfill('0') << ans << '\n';
            continue;
        }

        // build matrix [[K, L], [1,0]] and raise to power M-1
        Mat T(k, l, 1, 0);
        Mat P = matPow(T, M - 1ULL);

        // [a_M] = P * [a_1; a_0]  where a1 = K, a0 = 1
        // a_M = P.a00 * a1 + P.a01 * a0
        ull a1 = k; // reduced
        ull a0 = 1 % MOD;
        ull ans = ((P.a00 * a1) + (P.a01 * a0)) % MOD;

        cout << setw(6) << setfill('0') << ans << '\n';
    }

    return 0;
}
