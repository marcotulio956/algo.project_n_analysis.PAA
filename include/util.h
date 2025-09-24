#pragma once

#include <iostream>
#include <vector>

class Util {
public:
	static void print_vector(const std::vector<int> &arr) {
		for (const auto &num : arr) {
			std::cout << num << " ";
		}
		std::cout << std::endl;
	}
};