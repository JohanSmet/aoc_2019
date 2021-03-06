#include <algorithm>
#include <cassert>
#include <limits>
#include <iostream>
#include <vector>

using namespace std;

vector<int> read_program_source() {

	vector<int> result;
	int v;
	char delim;
	
	while (cin >> v) {
		result.push_back(v);
		cin >> delim;
	}

	return result;
}

int run_program(vector<int> &program, const vector<int> &input, vector<int> &output) {
	int ip = 0;
	int input_idx = 0;
	
	auto opcode = [](int v) -> int {return v % 100;};
	auto get_param = [&](int i) -> int {
		assert(i >= 1 && i <= 3);
		static const int factor[] = {1, 100, 1000, 10000};
		auto mode = (program[ip] / factor[i]) % 10;
		if (mode == 1) {
			return program[ip+i];
		} else {
			return program[program[ip+i]];
		}
	};

	auto set_param = [&](int i, int v) {
		program[program[ip+i]] = v;
	};

	while (ip < program.size()) {

		switch (opcode(program[ip])) {
			case 1: {		// addition
				int result = get_param(1) + get_param(2);
				set_param(3, result);
				ip += 4;
				break;
			}
			case 2: {		// multiplication
				int result = get_param(1) * get_param(2);
				set_param(3, result);
				ip += 4;
				break;
			}
			case 3:			// input
				set_param(1, input[input_idx++]);
				ip += 2;
				break;
			case 4:			// output
				output.push_back(get_param(1));
				ip += 2;
				break;
			case 5:	{		// jump-if-true
				int cond = get_param(1);
				if (cond != 0) {
					ip = get_param(2);
				} else {
					ip += 3;
				}
				break;
			}
			case 6:	{		// jump-if-false
				int cond = get_param(1);
				if (cond == 0) {
					ip = get_param(2);
				} else {
					ip += 3;
				}
				break;
			}
			case 7: 		// less than
				set_param(3, get_param(1) < get_param(2) ? 1 : 0);
				ip += 4;
				break;
			case 8: 		// equals
				set_param(3, get_param(1) == get_param(2) ? 1 : 0);
				ip += 4;
				break;

			case 99: return 0;
			default:
				cerr << "Invalid opcode: " << opcode(program[ip]) << endl;
				return -1;
		}
	}

	return -2;
}

ostream &operator <<(ostream &os, const vector<int> &v) {
	os << "<";
	for (auto i : v) {
		os << " " << i;
	}
	os << " >";
	return os;
}

int main() {
	auto clean_program = read_program_source();

	vector<int> phase_settings = {0, 1, 2, 3, 4};

	int max_output = numeric_limits<int>::min();
	vector<int> best_settings;

	do {
		int signal = 0;

		for (int i = 0; i < 5; ++i) {
			vector<int> outputs;
			auto program = clean_program;

			if (run_program(program, {phase_settings[i], signal}, outputs) != 0) {
				cout << "Error running program" << endl;
				return -1;
			}

			signal = outputs.front();
		}

		if (signal > max_output) {
			max_output = signal;
			best_settings = phase_settings;
		}

	} while (next_permutation(begin(phase_settings), end(phase_settings)));

	cout << "Max thruster signal: " << max_output << endl;
	cout << "Phase settings: " << best_settings << endl;

	return 0;
}
