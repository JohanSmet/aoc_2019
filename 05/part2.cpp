#include <algorithm>
#include <cassert>
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
			case 8: 		// less than
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

ostream &operator <<(ostream &os, const vector<int> &program) {
	os << "<";
	for (auto v : program) {
		cout << " " << v;
	}
	os << " >";
	return os;
}

int main() {
	// load program
	auto program = read_program_source();

	// run program 
	vector<int> output;
	run_program(program, {5}, output);

	cout << "Output: " << output << endl;
	return 1;
}
