#include <fstream>
#include <iostream>

namespace {

static const char *INPUT_FILE = "input.txt";

} // unnamed namespace

int main() {
	std::ifstream input(INPUT_FILE, std::ios::in);
	if (!input.is_open()) {
		std::cout << "Error opening file " << INPUT_FILE << std::endl;
		return -1;
	}
	
	int mass;
	int total_fuel = 0;

	while (input >> mass) {
		total_fuel += (mass / 3) - 2;
	}

	std::cout << "Total fuel required = " << total_fuel << std::endl;

	return 0;
}
