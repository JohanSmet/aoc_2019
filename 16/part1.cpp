#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

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

int main() {
	vector<int8_t> input;

	// read source
	char c;
	while (cin >> c) {
		input.push_back(c - '0');
	}

	// run
	for (int step = 1; step <= 100; ++step) {
		vector<int8_t> output = fft(input);
		input = output;
	}

	// display
	for (int i = 0; i < 8; ++i) {
		cout << static_cast<char>('0' + input[i]);
	}
	cout << endl;
}
