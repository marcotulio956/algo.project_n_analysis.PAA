#pragma once

#include <iostream>
#include <vector>

class Sortings {
public:
	Sortings();
	~Sortings();
	void static insertion_sort(std::vector<int>& arr);
	void static selection_sort(std::vector<int>& arr, bool swaponce);

	std::vector<int> static merge(std::vector<int>& vet1, std::vector<int>& vet2);
	void static merge_sort_main(std::vector<int>& arr);
	void static merge_sort(std::vector<int>& arr);
	int static find_greatest(std::vector<int>& arr);
	int static find_2ndgreatest(std::vector<int>& arr);
	int static find_nthgreatest(std::vector<int>& arr, int n);
};