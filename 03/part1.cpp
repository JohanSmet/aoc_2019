#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

struct point_t {
	int x;
	int y;
};

struct segment_t {
	bool horizontal;
	point_t ends[2];
};

using wire_t = vector<segment_t>;

wire_t parse_wire(const string &src) {

	stringstream ss(src);
	wire_t result;

	char dir;
	int len;
	char delim;
	point_t start = {0,0};
	
	while (ss >> dir) {
		ss >> len;
		ss >> delim;

		segment_t s;
		s.ends[0] = start;
		s.ends[1] = start;
		switch (dir) {
			case 'L':
				s.ends[1].x -= len;
				s.horizontal = true;
				break;
			case 'R':
				s.ends[1].x += len;
				s.horizontal = true;
				break;
			case 'U':
				s.ends[1].y -= len;
				s.horizontal = false;
				break;
			case 'D':
				s.ends[1].y += len;
				s.horizontal = false;
				break;
		}

		start = s.ends[1];

		if (s.ends[0].x > s.ends[1].x || s.ends[0].y > s.ends[1].y) {
			swap(s.ends[0], s.ends[1]);
		}
		
		result.push_back(s);
	}

	return result;
}

bool intersect_segments(const segment_t &s0, const segment_t &s1, point_t &cross) {
	
	// parallel segments don't cross
	if (s0.horizontal == s1.horizontal) {
		return false;
	}

	auto hor = s0;
	auto ver = s1;
	if (!hor.horizontal) {
		swap(hor, ver);
	}

	if (ver.ends[0].x >= hor.ends[0].x && ver.ends[0].x <= hor.ends[1].x &&
		hor.ends[0].y >= ver.ends[0].y && hor.ends[0].y <= ver.ends[1].y) {
		cross.x = ver.ends[0].x;
		cross.y = hor.ends[0].y;
		return true;
	}
	
	return false;
}

int closest_intersection(const wire_t &w0, const wire_t &w1) {
	int min_dist = numeric_limits<int>::max();

	for (const auto &s1 : w1) {
		for (const auto &s0 : w0) {
			point_t intersection;
			if (intersect_segments(s0, s1, intersection)) {
				auto dist = abs(intersection.x) + abs(intersection.y);
				if (dist > 0) {
					min_dist = min(min_dist, dist);
				}
			}
		}
	}

	return min_dist;
}

int main() {
	// read input
	std::string w0s, w1s;
	getline(cin, w0s);
	getline(cin, w1s);

	// parse wires
	auto w0 = parse_wire(w0s);
	auto w1 = parse_wire(w1s);

	// find intersection
	cout << closest_intersection(w0, w1) << endl;

	return 0;
}
