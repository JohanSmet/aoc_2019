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

int main() {
	vector<Moon> moons;

	read_scan(moons);

	print_moons(0, moons);

	for (int step = 1; step <= 1000; ++step) {
		simulate(moons);
		print_moons(step, moons);
	}

	cout << endl << "Energy in system = " << calculate_energy(moons) << endl;

	return 0;
}
