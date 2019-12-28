#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>
#include <regex>
#include <numeric>

using namespace std;

using vec_t = int[3];

struct Moon {
	vec_t position;
	vec_t velocity;
};

void read_scan(vector<Moon> &moons) {

	string line;
	
	std::regex pos_regex("([-]*[\\d]+)");
	Moon moon_templ = {0};
	
	while (getline(cin, line)) {
		auto reg_begin = sregex_iterator(begin(line), end(line), pos_regex);
		auto reg_end = sregex_iterator();
		int i = 0;

		for (auto iter = reg_begin; iter != reg_end; ++iter) {
			moon_templ.position[i++] = stoi(iter->str());
		}

		moons.push_back(moon_templ);
	}
}

ostream &operator<<(ostream &os, const vec_t &vec) {
	os << "<x=" << setw(3) << vec[0] 
	   << ", y=" << setw(3) << vec[1] 
	   << ", z=" << setw(3) << vec[2] << ">";
	return os;
}

void print_moons(int steps, const vector<Moon> &moons) {
	
	cout << "After " << steps << " steps:" << endl;
	
	for (const auto &m : moons) {
		cout << "pos=" << m.position << "vel=" << m.velocity << endl;
	}
}

void simulate(vector<Moon> &moons) {
	// apply gravity
	for (int i = 0; i < moons.size(); ++i) {
		for (int j = i + 1; j < moons.size(); ++j) {
			for (int k = 0; k < 3; k++) {
				if (moons[i].position[k] < moons[j].position[k]) {
					moons[i].velocity[k] += 1;
					moons[j].velocity[k] -= 1;
				} else if (moons[i].position[k] > moons[j].position[k]) {
					moons[i].velocity[k] -= 1;
					moons[j].velocity[k] += 1;
				} 
			}
		}
	}

	// apply velocity
	for (auto &m : moons) {
		m.position[0] += m.velocity[0];	
		m.position[1] += m.velocity[1];	
		m.position[2] += m.velocity[2];	
	}
}

int calculate_energy(const vector<Moon> &moons) {

	static const auto abs_sum = [](auto x, auto y) {return abs(x) + abs(y);};
	
	int result = 0;

	for (auto &m : moons) {
		auto pot = accumulate(begin(m.position), end(m.position), 0, abs_sum);
		auto kin = accumulate(begin(m.velocity), end(m.velocity), 0, abs_sum);
		result += pot * kin;
	}

	return result;
}

bool axis_match_initial(const vector<Moon> &cur, const vector<Moon> &initial, int axis) {
	bool result = true;

	for (int i = 0; result && i < cur.size(); ++i) {
		result &= cur[i].position[axis] == initial[i].position[axis] &&
				  cur[i].velocity[axis] == initial[i].velocity[axis];
	}

	return result;
}

int main() {
	vector<Moon> moons;

	read_scan(moons);

	auto initial_state = moons;
	print_moons(0, initial_state);

	// the functions for the three axii are independent. This means that we need to calculate the period of each axis.
	// Once we have those we need to find a point were these periods match up, which is their lowest common multiple.
	// Luckily for us C++ includes a function to calculate the LCM (available since C++17).

	int64_t axis_period[3] = {-1, -1, -1};
	int axis_count = 0;

	for (int step = 1; true; ++step) {
		simulate(moons);

		for (int axis = 0; axis < 3; ++axis) {
			if (axis_period[axis] > -1) continue;
			if (axis_match_initial(moons, initial_state, axis)) {
				axis_period[axis] = step;
				axis_count += 1;
			}
		}

		if (step == 1000) {
			print_moons(step, moons);
			cout << "Energy in system at step " << step << " = " << calculate_energy(moons) << endl;
		}

		if (step >= 1000 && axis_count == 3) {
			break;
		}
	}

	cout << "Steps required before loop = " << lcm(lcm(axis_period[0], axis_period[1]), axis_period[2]) << endl;

	return 0;
}
