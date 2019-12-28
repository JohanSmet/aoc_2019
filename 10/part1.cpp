#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

struct Asteroid {
	int x;
	int y;
	int los;
};

void read_map(vector<Asteroid> &asteroids) {

	string line;
	int y = 0;
	
	while (getline(cin, line)) {
		for (int x = 0; x < line.size(); ++x) {
			if (line[x] == '#')  {
				asteroids.push_back({x, y, 0});
			}
		}
		y += 1;
	}
}

void count_los_asteriods(Asteroid &src, const vector<Asteroid> &asteroids) {
// find the number of asteriods with a direct line-of-sight to the specified asteroid
//	-> two asteroids block each others line-of-sight if the angle between the origin and them is the same
//	-> build a set of unique angles from the source to all the other asteroids

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

	read_map(asteroids);

	for (auto &a : asteroids) {
		count_los_asteriods(a, asteroids);
	}

	auto best = max_element(begin(asteroids), end(asteroids), [](auto a, auto b) {return a.los < b.los;});
	cout << "Best at (" << best->x << "," << best->y << ") with " << best->los << " visible asteroids" << endl;

	return 0;
}
