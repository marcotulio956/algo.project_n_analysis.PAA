#pragma once
#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Util {
public:
	static void print_vector(const std::vector<int> &arr) {
		for (const auto &num : arr) {
			std::cout << num << " ";
		}
		std::cout << std::endl;
	}
};

struct EdgeInfo {
    string label; // name/label of the edge
    double weight;     // numeric weight (for shortest-path)
    int capacity;      // arbitrary integer property
    bool active;       // boolean flag
};

// helper to print edge info
inline ostream& operator<<(ostream& os, const EdgeInfo& e) {
    os << "{label:" << e.label
       << ", weight:" << e.weight
       << ", cap:" << e.capacity
       << ", active:" << (e.active ? "Y" : "N") << "}";
    return os;
}

#endif // UTIL_HPP