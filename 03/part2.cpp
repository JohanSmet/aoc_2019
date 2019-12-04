#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct point_t {
	int x;
	int y;
};

struct segment_t {
	bool horizontal;
	int steps;
	int first;
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
	int total_steps = 0;
	
	while (ss >> dir) {
		ss >> len;
		ss >> delim;

		segment_t s;
		s.steps = total_steps;
		s.first = 0;
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
		total_steps += len;

		if (s.ends[0].x > s.ends[1].x || s.ends[0].y > s.ends[1].y) {
			swap(s.ends[0], s.ends[1]);
			s.first = 1;
		}
		
		result.push_back(s);
	}

	return result;
}

void dump_wire(const wire_t &w) {
	for (const auto &s : w) {
		cout << s.horizontal << "(" << s.ends[0].x << "," << s.ends[0].y << ")"
							 << "(" << s.ends[1].x << "," << s.ends[1].y << ")"
			 << endl;
	}
}

bool intersect_segments(const segment_t &s0, const segment_t &s1, point_t &cross, int &h_steps, int &v_steps) {
	
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

		h_steps = abs(cross.x - hor.ends[hor.first].x);
		v_steps = abs(cross.y - ver.ends[ver.first].y);

		return true;
	}
	
	return false;
}

int closest_intersection(const wire_t &w0, const wire_t &w1) {
	int min_dist = numeric_limits<int>::max();

	for (const auto &s1 : w1) {
		for (const auto &s0 : w0) {
			point_t intersection;
			int h_steps, v_steps;
			if (intersect_segments(s0, s1, intersection, h_steps, v_steps)) {
				auto dist = s0.steps + s1.steps + h_steps + v_steps;
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
	ifstream is("input.txt", ios::in);
	getline(is, w0s);
	getline(is, w1s);
	is.close();

	//string w0s = "R75,D30,R83,U83,L12,D49,R71,U7,L72";
	//string w1s = "U62,R66,U55,R34,D71,R55,D58,R83";
	//string w0s = "R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51";
	//string w1s = "U98,R91,D20,R16,D67,R40,U7,R15,U6,R7";

	// parse wires
	auto w0 = parse_wire(w0s);
	auto w1 = parse_wire(w1s);

	// find intersection
	cout << closest_intersection(w0, w1) << endl;

	return 0;
}
