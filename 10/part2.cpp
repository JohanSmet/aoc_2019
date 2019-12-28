#include <cmath>
#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>

using namespace std;

struct Asteroid {
	int x;
	int y;
	int los;
	int dist_squared;
};

void read_map(vector<Asteroid> &asteroids) {

	string line;
	int y = 0;
	
	while (getline(cin, line)) {
		for (int x = 0; x < line.size(); ++x) {
			if (line[x] == '#')  {
				asteroids.push_back({x, y, 0, 0});
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

	read_map(asteroids);

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
		rank += 1;

		if (rank == 200) {
			cout << "Asteroid " << rank << " vaporized at " << iter->second[0]->x << "," << iter->second[0]->y << endl;
			break;
		}
	}

	return 0;
}
