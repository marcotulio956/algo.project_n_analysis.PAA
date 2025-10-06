#include "usecases/sortings/usesortings.hpp"
#include "util.hpp"
#include "sortings.hpp"
#include <iostream>
#include <string>

using namespace Sortings;

void use_sortings() {
    vector<int> arr = {64, 25, 12, 22, 11, 07, 90, 45, 23, 67, 89};
    cout << "original array: "; Util::print_vector(arr);
    insertion_sort(arr);

    arr = {13, 46, 24, 52, 20, 9, 78, 34, 56, 12, 01};
    cout << "original array: "; Util::print_vector(arr);
    selection_sort(arr, false);

    arr = {23, 45, 12, 67, 34, 89, 90, 11, 78, 56, 01};
    cout << "original array: "; Util::print_vector(arr);
    merge_sort(arr);

    arr = {1, 2, 3};
    cout << "greatest: " << find_greatest(arr) << endl;
    cout << "greatest2: " << find_2ndgreatest(arr) << endl;

    arr = {3 , 2, 1};
    cout << "greatest: " << find_greatest(arr) << endl;
    cout << "greatest2: " << find_2ndgreatest(arr) << endl;

    arr = {3};
    cout << "greatest: " << find_greatest(arr) << endl;
    cout << "greatest2: " << find_2ndgreatest(arr) << endl;

    arr = {3, 3, 3};
    cout << "greatest: " << find_greatest(arr) << endl;
    cout << "greatest2: " << find_2ndgreatest(arr) << endl;

    arr = {3, 3, 2, 1, 1, 2};
    for (size_t i=0; i<arr.size(); i++) {
        cout<< "nth greatest: " << i+1 << " is " << find_nthgreatest(arr, i+1) << endl;
    }
}