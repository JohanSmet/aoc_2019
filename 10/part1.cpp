#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <cmath>

using namespace std;

static const char *INPUT_FILE = "input.txt";

struct Asteroid {
	int x;
	int y;
	int los;
};

bool read_map(vector<Asteroid> &asteroids) {

	ifstream is(INPUT_FILE, ios::in);
	if (!is.is_open()) {
		cerr << "Error opening file " << INPUT_FILE << endl;
		return false;
	}

	string line;
	int y = 0;
	
	while (is >> line) {
		for (int x = 0; x < line.size(); ++x) {
			if (line[x] == '#')  {
				asteroids.push_back({x, y, 0});
			}
		}
		y += 1;
	}

	return true;
}

void count_los_asteriods(Asteroid &src, const vector<Asteroid> &asteroids) {

	unordered_set<double> angles;

	for (auto dst : asteroids) {
		if (&src == &dst) continue;
		double angle = atan2(dst.y - src.y, dst.x - src.x);
		angles.insert(angle);
	}

	src.los = angles.size();
}

int main() {
	vector<Asteroid> asteroids;

	if (!read_map(asteroids)) {
		return -1;
	}

	for (auto &a : asteroids) {
		count_los_asteriods(a, asteroids);
	}

	auto best = max_element(begin(asteroids), end(asteroids), [](auto a, auto b) {return a.los < b.los;});
	cout << "Best at (" << best->x << "," << best->y << ") with " << best->los << " visible asteroids" << endl;

	return 0;
}
