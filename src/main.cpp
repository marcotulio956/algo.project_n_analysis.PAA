#include "sortings.h"
#include "util.h"

int main(){
	std::vector<int> arr = {64, 25, 12, 22, 11, 07, 90, 45, 23, 67, 89};
	std::cout << "original array: "; Util::print_vector(arr);
	Sortings::insertion_sort(arr);

	arr = {13, 46, 24, 52, 20, 9, 78, 34, 56, 12, 01};
	std::cout << "original array: "; Util::print_vector(arr);
	Sortings::selection_sort(arr, false);

	arr = {23, 45, 12, 67, 34, 89, 90, 11, 78, 56, 01};
	std::cout << "original array: "; Util::print_vector(arr);
	Sortings::merge_sort(arr);

	arr = {1, 2, 3};
	std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
	std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;
	arr = {3 , 2, 1};
	std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
	std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;
	arr = {3};
	std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
	std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;
	arr = {3, 3, 3};
	std::cout << "greatest: " << Sortings::find_greatest(arr) << std::endl;
	std::cout << "greatest2: " << Sortings::find_2ndgreatest(arr) << std::endl;
	arr = {3, 3, 2, 1, 1, 2};
	for (size_t i=0; i<arr.size(); i++) {
		std::cout<< "nth greatest: " << i+1 << " is " << Sortings::find_nthgreatest(arr, i+1) << std::endl;
	}
}	