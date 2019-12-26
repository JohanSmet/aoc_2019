#include <algorithm>
#include <iostream>
#include <bitset>
#include <map>
#include <numeric>

using namespace std;

using scan_level_t = bitset<25>;
using scan_t = map<int, scan_level_t>;

scan_t read_input() {
	int i = 0;
	char c;
	scan_level_t level;

	while (cin >> c) {
		level.set(i++, c == '#');
	}

	scan_t scan;
	scan.emplace(0, level);

	return scan;
}

scan_t simulate(scan_t input) {

	if (input.begin()->second.count() > 0) {
		input[input.begin()->first - 1] = 0;
	}

	if (input.rbegin()->second.count() > 0) {
		input[input.rbegin()->first + 1] = 0;
	}

	auto result = input;

	auto val = [&](const scan_t &scan, int d, int x, int y, int ox, int oy) -> int {
		const auto &current = scan.find(d)->second;
		if (x == 2 && y == 2) {
			// level down (+1)
			auto found = scan.find(d+1);
			if (found == end(scan)) {
				return false;
			}
			const auto &other = found->second;
			if (ox == 1) {
				return other[0] + other[5] + other[10] + other[15] + other[20];
			} else if (ox == 3) {
				return other[4] + other[9] + other[14] + other[19] + other[24];
			} else if (oy == 1) {
				return other[0] + other[1] + other[2] + other[3] + other[4];
			} else if (oy == 3) {
				return other[20] + other[21] + other[22] + other[23] + other[24];
			}
			return false;
		} else if (x >= 0 && x <= 4 && y >= 0 && y <= 4) {
			return current[(y * 5) + x];
		} else {
			// level up (-1)
			auto found = scan.find(d-1);
			if (found == end(scan)) {
				return false;
			}
			const auto &other = found->second;
			if (x < 0) {
				return other[11];
			} else if (x > 4) {
				return other[13];
			} else if (y < 0) {
				return other[7];
			} else if (y > 4) {
				return other[17];
			}
			return false;
		}
	};

	for (auto [depth, level] : input) {
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				if (x == 2 && y == 2) continue;
				int neighbours = val(input, depth, x-1, y, x, y);
				neighbours += val(input, depth, x+1, y, x, y);
				neighbours += val(input, depth, x, y-1, x, y);
				neighbours += val(input, depth, x, y+1, x, y);

				auto has_bug = val(input, depth, x,y, x, y);
				if (has_bug && neighbours != 1) {
					result[depth].reset((y*5)+x);
				} 
				if (!has_bug && (neighbours == 1 || neighbours == 2)) {
					result[depth].set((y*5)+x);
				}
			}
		}
	}

	return result;
}

int main() {

	auto result = read_input();
	
	for (int i = 1; i <= 200; ++i) {
		result = simulate(result);

		if (i == 10 || i == 200) {
			cout << "Bugs after " << i << ": " 
				 << accumulate(begin(result), end(result), 0, [](auto a, auto b) {return a + b.second.count();}) 
				 << endl;
		}
	}

	return 0;
}
