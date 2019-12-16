#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <functional>
#include <algorithm>

using namespace std;

static const char *INPUT_FILE = "input.txt";

vector<int8_t> fft(const vector<int8_t> &input) {
	vector<int8_t> output;
	output.reserve(input.size());
	
	static const int8_t pattern[] = {0, 1, 0, -1};

	for (int i = 0; i < input.size(); ++i) {
		int temp = 0;

		for (int j = 0; j < input.size(); ++j) {
			int p = ((j+1) / (i+1)) % 4;
			temp += input[j] * pattern[p];
		}
		
		output.push_back(abs(temp) % 10);
	}

	return output;
}

void dump_vector(const vector<int8_t> &v) {
	for (auto i : v) {
		cout << (char)('0' + i);
	}
	cout << endl;
}

int main() {
	vector<int8_t> input;

	// read source
	std::string source;
	ifstream is(INPUT_FILE, ios::in);
	if (!is.is_open()) {
		cerr << "Error opening file" << endl;
		return -1;
	}
	char c;
	while (is >> c) {
		input.push_back(c - '0');
	}
	is.close();

	// run
	for (int step = 1; step <= 100; ++step) {
		vector<int8_t> output = fft(input);
		input = output;
	}

	dump_vector(input);
}
