#include <bitset>
#include <iostream>
#include <vector>

using namespace std;

using scan_t = bitset<25>;

vector<scan_t> history;

void read_input() {
	int i = 0;
	char c;
	scan_t scan;

	while (cin >> c) {
		scan.set(i++, c == '#');
	}

	history.push_back(scan);
}

scan_t simulate(scan_t input) {
	scan_t result = input;

	auto val = [&](int x, int y) -> bool {
		if (x >= 0 && x <= 4 && y >= 0 && y <= 4) {
			return input[(y * 5) + x];
		} else {
			return false;
		}
	};

	for (int y = 0; y < 5; ++y) {
		for (int x = 0; x < 5; ++x) {
			int neighbours = val(x-1, y);
			neighbours += val(x+1, y);
			neighbours += val(x, y-1);
			neighbours += val(x, y+1);

			auto has_bug = val(x,y);
			if (has_bug && neighbours != 1) {
				result.reset((y*5)+x);
			} 
			if (!has_bug && (neighbours == 1 || neighbours == 2)) {
				result.set((y*5)+x);
			}
		}
	}

	return result;
}

int main() {

	read_input();
	
	while (true) {
		auto scan = simulate(history.back());

		auto found = find(begin(history), end(history), scan);
		if (found != end(history)) {
			cout << "Biodiversity = " << scan.to_ulong() << endl;
			break;
		}

		history.push_back(scan);
	}

	return 0;
}
