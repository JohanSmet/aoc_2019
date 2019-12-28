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
	vector<char> result;
	copy_n(begin(image), IMG_LAYER_SIZE, back_inserter(result));

	for (auto layer = begin(image) + IMG_LAYER_SIZE; layer != end(image); layer += IMG_LAYER_SIZE) {
		for (int i = 0; i < IMG_LAYER_SIZE; i++) {
			if (result[i] == '2') {
				result[i] = *(layer + i);
			}
		}
	}

	// display image
	for (int i = 0; i < IMG_LAYER_SIZE; ++i) {
		cout << (result[i] == '1' ? '#' : ' ');
		if ((i + 1) % IMG_WIDTH == 0) 
			cout << endl;
	}

	return 0;
}
