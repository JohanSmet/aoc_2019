#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>

using namespace std;

static const char *INPUT_FILE = "input.txt";

using program_t = vector<int>;

class Computer {
public:
	// functions
	Computer(const program_t &prog) : program(prog) {
	}

	void run_until_output(const vector<int> &input);

	// data
	program_t	program;
	int			ip = 0;
	int			output = 0;
	bool		finished = false;
	bool		error = false;
};


program_t parse_program_source(const string &src) {

	stringstream ss(src);

	program_t result;
	int v;
	char c;
	
	while (ss >> v) {
		result.push_back(v);
		ss >> c;
	}

	return result;
}

void Computer::run_until_output(const vector<int> &input) {
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
				output = get_param(1);
				ip += 2;
				return;
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
			case 8: 		// less than
				set_param(3, get_param(1) == get_param(2) ? 1 : 0);
				ip += 4;
				break;
			case 99: 
				finished = true;
				return;
			default:
				cerr << "Invalid opcode: " << opcode(program[ip]) << endl;
				finished = true;
				error = true;
				return;
		}
	}
}

void dump_vector(const char *label, const vector<int> &program) {

	cout << label;
	for (auto v : program) {
		cout << " " << v;
	}
	cout << endl;
}

int main() {
	// read source
	std::string source;
	ifstream is(INPUT_FILE, ios::in);
	is >> source;
	is.close();

	// parse program
	auto program = parse_program_source(source); 

	vector<int> phase_settings = {5, 6, 7, 8, 9};

	int max_output = numeric_limits<int>::min();
	vector<int> best_settings;

	do {
		int signal = 0;
		Computer amplifiers[5] = {program, program, program, program, program};

		// apply settings and start amplifiers
		for (int i = 0; i < 5; ++i) {
			amplifiers[i].run_until_output({phase_settings[i], signal});
			signal = amplifiers[i].output;
		}
		
		// run amplifiers until they finish
		while (!amplifiers[0].finished) {
			for (int i = 0; i < 5; ++i) {
				amplifiers[i].run_until_output({signal});
				if (!amplifiers[i].finished) {
					signal = amplifiers[i].output;
				}
			}
		}

		if (signal > max_output) {
			max_output = signal;
			best_settings = phase_settings;
		}

	} while (next_permutation(begin(phase_settings), end(phase_settings)));

	cout << "Max thruster signal: " << max_output << endl;
	dump_vector("Phase settings: ", best_settings);

	return 0;
}
