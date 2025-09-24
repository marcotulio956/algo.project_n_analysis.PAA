#include "sortings.h"
#include "util.h"

Sortings::Sortings() {}

Sortings::~Sortings() {}

void Sortings::insertion_sort(std::vector<int>& arr) {
	for (size_t j=1; j<arr.size(); j++){
		int key = arr[j];
		size_t i = j - 1;
		while (i < arr.size() && arr[i] > key) {
			arr[i + 1] = arr[i];
			i--;
		}
		arr[i + 1] = key;
	}
	std::cout << "insertion sort: "; Util::print_vector(arr);
}

void Sortings::selection_sort(std::vector<int>& arr, bool swaponce=false) { 
	for (size_t j=0; j<arr.size()-1; j++){
		size_t min_index = j;
		for (size_t i=j+1; i<arr.size(); i++){
			if (arr[i] < arr[j]) {
				min_index = i;
				if (!swaponce){
					std::swap(arr[j], arr[i]);
				}
			}
		}
		if (swaponce && min_index != j) {
			std::swap(arr[j], arr[min_index]);
		}
	}
	std::cout << "selection sort: "; Util::print_vector(arr);
}

std::vector<int> Sortings::merge(std::vector<int>& vet1, std::vector<int>& vet2) {
	std::vector<int> merged;
	size_t i=0, j=0;
	// std::cout << "merge: "; 
	// Util::print_vector(vet1);
	// Util::print_vector(vet2);

	while (i < vet1.size() && j < vet2.size() ) {
		if (vet1[i] < vet2[j]) {
			merged.push_back(vet1[i]);
			i++;
		} else {
			merged.push_back(vet2[j]);
			j++;
		}
	}
	while (i < vet1.size()) {
		merged.push_back(vet1[i]);
		i++;
	}
	while (j < vet2.size()) {
		merged.push_back(vet2[j]);
		j++;
	}
	return merged;
}

/*
T(n) = 2T(n/2) + O(n) p/  n>1
T(1) = O(1)           p/  n=1
*/

void Sortings::merge_sort_main(std::vector<int>& arr) {
	// printf("merge sort: ");
	// Util::print_vector(arr);
	if (arr.size() <= 1) {
		std::cout << "merged array: "; Util::print_vector(arr);
		return;
	}
	// mid is the floor of arr.size()/2
	int arr_size = arr.size();
	int mid = arr_size/2;
	// std::cout << "mid: " << mid << std::endl;
	std::vector<int> left(arr.begin(), arr.begin() + mid);
	std::vector<int> right(arr.begin() + mid, arr.end());
	// Util::print_vector(left);
	// Util::print_vector(right);
	merge_sort_main(left); 
	merge_sort_main(right);
	
	std::vector<int> merged;
	merged = merge(left, right);

	arr = merged;
	// std::cout << "merged array: "; Util::print_vector(arr);
}

void Sortings::merge_sort(std::vector<int>& arr) {
	merge_sort_main(arr);
	std::cout << "merge sort: "; Util::print_vector(arr);
}

int Sortings::find_greatest(std::vector<int>& arr) {
	int greatest = arr[0];
	for (size_t i=1; i<arr.size(); i++) {
		if (arr[i] > greatest) {
			greatest = arr[i];
		}
	}
	return greatest;
}

int Sortings::find_2ndgreatest(std::vector<int>& arr) {
	int greatest = arr[0];
	int greatest2 = 0; // neg inf 
	for (size_t i=1; i<arr.size(); i++) {
		if (arr[i] > greatest) {
			greatest2 = greatest;
			greatest = arr[i];
		} 
		// either: 1
		else if (/*arr[i] <= greatest && */arr[i] > greatest2) {
			greatest2 = arr[i];
		}
		// or: 2		
		// if (arr[i] > greatest2 && arr[i] <= greatest) {
		// 	greatest2 = arr[i];
		// }
	}
	return greatest2;
}

int Sortings::find_nthgreatest(std::vector<int>& arr, int n) {
	std::vector<int> arr_sorted = arr;

	Sortings::merge_sort(arr_sorted); // O(n log n)

	return arr_sorted[arr_sorted.size() - n]; // O(1)
}

