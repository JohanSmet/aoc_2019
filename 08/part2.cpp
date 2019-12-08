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

void dump_vector(vector<char>::const_iterator b, vector<char>::const_iterator e) {
	for (auto i = b; i != e; ++i) {
		char o = (*i == '1') ? '@' : ' ';
		cout << o;
	}
	cout << endl;
}

int main() {
	vector<char> image;

	// read source
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
	vector<char> result;
	copy_n(begin(image), IMG_LAYER_SIZE, back_inserter(result));

	for (auto layer = begin(image) + IMG_LAYER_SIZE; layer != end(image); layer += IMG_LAYER_SIZE) {
		for (int i = 0; i < IMG_LAYER_SIZE; i++) {
			if (result[i] == '2') {
				result[i] = *(layer + i);
			}
		}
	}

	for (int r = 0; r < IMG_HEIGHT; ++r) {
		auto b = begin(result) + (r * IMG_WIDTH);
		auto e = b + IMG_WIDTH;
		dump_vector(b, e);
	}

	return 0;
}
