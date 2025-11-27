#include <bits/stdc++.h>

using namespace std;

int main(){
	int N=-1, D=-1;
	while(true){
		cin >> N >> D;

		if(N==0 && D==0)
			return 0;
	
		string s;
		cin >> s;
		int rm = D;

		vector<char> st;
		st.reserve(N);

		for (char c : s) {
			while (!st.empty() && rm > 0 && st.back() < c) {
				st.pop_back();
				rm--;
			}
			st.push_back(c);
		}

		while (rm > 0) {
			st.pop_back();
			rm--;
		}

		for (int i = 0; i < N - D; i++)
			cout << st[i];
		cout << "\n";
		
	}
}
