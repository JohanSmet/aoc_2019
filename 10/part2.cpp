#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_set>
#include <map>
#include <limits>
#include <algorithm>
#include <cmath>

using namespace std;

static const char *INPUT_FILE = "input.txt";

struct Asteroid {
	int x;
	int y;
	int los;
	int dist_squared;
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
				asteroids.push_back({x, y, 0, 0});
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

map<double, vector<Asteroid *>> create_angle_lookup(Asteroid &station, vector<Asteroid> &asteroids) {
	map<double, vector<Asteroid *>> result;

	for (auto &dst : asteroids) {
		if (&station == &dst) continue;
		auto dx = dst.x - station.x;
		auto dy = dst.y - station.y;
		double angle = atan2(dx, dy);

		dst.dist_squared = dx * dx + dy * dy;

		result[angle].push_back(&dst);
	}

	for (auto &node : result) {
		sort(begin(node.second), end(node.second), [](auto a, auto b) {return a->dist_squared < b->dist_squared;});
	}

	return result;
}


int main() {
	vector<Asteroid> asteroids;

	if (!read_map(asteroids)) {
		return -1;
	}

	// part1: find best location for monitoring station
	for (auto &a : asteroids) {
		count_los_asteriods(a, asteroids);
	}

	auto station = max_element(begin(asteroids), end(asteroids), [](auto a, auto b) {return a.los < b.los;});
	cout << "Monitoring at at (" << station->x << "," << station->y << ") with " << station->los << " visible asteroids" << endl;

	// part2: vaporize asteroids 
	//	taking a shortcut here: we're only interested in the 200th asteroid and we know from part1 that there are 284 visible asteroids
	//  from the chosen station. So we don't need to worry about the second iteration to eliminate the remaining asteroids
	auto shooter = create_angle_lookup(*station, asteroids);
	int rank = 0;

	for (auto iter = rbegin(shooter); iter != rend(shooter); ++iter) {
		cout << ++rank << ": " << iter->first << ": " << iter->second[0]->x << "," << iter->second[0]->y << endl;
	}

	return 0;
}
