#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>

using namespace std;

static const char *INPUT_FILE = "input.txt";
static constexpr int IMG_WIDTH  = 25;
static constexpr int IMG_HEIGHT = 6;
static constexpr int IMG_LAYER_SIZE = IMG_WIDTH * IMG_HEIGHT;

void dump_vector(const char *label, vector<char>::const_iterator b, vector<char>::const_iterator e) {

	cout << label;
	for (auto i = b; i != e; ++i) {
		cout << " " << *i;
	}
	cout << endl;
}

int main() {
	vector<char> image;

	// read source
	std::string source;
	ifstream is(INPUT_FILE, ios::in);
	if (!is.is_open()) {
		cerr << "Error opening input" << endl;
		return -1;
	}
	char c;
	while (is >> c) {
		image.push_back(c);
	}

	is.close();

	// iterate through the layers
	auto layer_begin = begin(image);
	auto layer_end = layer_begin + IMG_LAYER_SIZE;
	int min_zeros = count(layer_begin, layer_end, '0');
	int result = count(layer_begin, layer_end, '1') * count(layer_begin, layer_end, '2');
	layer_begin = layer_end;

	while (layer_begin != end(image)) {
		auto layer_end = layer_begin + IMG_LAYER_SIZE; 
		int zeros = count(layer_begin, layer_end, '0');
		if (zeros < min_zeros) {
			result = count(layer_begin, layer_end, '1') * count(layer_begin, layer_end, '2');
			min_zeros = zeros;
		}
		layer_begin = layer_end;
	}

	cout << result << endl;

	return 0;
}
