#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

static constexpr int IMG_WIDTH  = 25;
static constexpr int IMG_HEIGHT = 6;
static constexpr int IMG_LAYER_SIZE = IMG_WIDTH * IMG_HEIGHT;

int main() {
	vector<char> image;

	// read source
	char c;
	while (cin >> c) {
		image.push_back(c);
	}

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
