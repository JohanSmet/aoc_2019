#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>

using namespace std;

// A key observation to make part 2 finish in a decent time frame is that due to the structure of the pattern
// the new value of an element in the second half is just the sum of the element and all the subsequent elements.
// When you start at the back of the sequency the result is just the partial sum.
// This solution only works when the key is in the second half of the repeated message.

vector<int> decode(const vector<int> &input) {
	vector<int> sums;
	sums.reserve(input.size());

	partial_sum(crbegin(input), crend(input), back_inserter(sums));

	vector<int> output;
	output.reserve(input.size());
	transform(crbegin(sums), crend(sums), back_inserter(output), [&](auto x) {return abs(x) % 10;});
	
	return output;
}

int main() {
	vector<int8_t> input_data;

	// read source
	char c;
	while (cin >> c) {
		input_data.push_back(c - '0');
	}

	// duplicate input
	vector<int> input;

	for (int i = 0; i < 10000; ++i) {
		input.insert(end(input), begin(input_data), end(input_data));
	}

	// get position of key
	int key_pos = input_data[0];
	for (int i = 1; i < 7; ++i) {
		key_pos = (key_pos * 10) + input_data[i];
	}

	if (key_pos * 2 < input.size()) {
		cerr << "Solution won't work for this input" << endl;
		return -1;
	}

	// ignore everything before the key
	input.erase(begin(input), begin(input) + key_pos);

	// run
	for (int step = 1; step <= 100; ++step) {
		auto output = decode(input);
		input = output;
	}

	// display
	for (int i = 0; i < 8; ++i) {
		cout << static_cast<char>('0' + input[i]);
	}
	cout << endl;
}
