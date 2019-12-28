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
			case 1: {
				int result = get_param(1) + get_param(2);
				set_param(3, result);
				ip += 4;
				break;
			}
			case 2: {
				int result = get_param(1) * get_param(2);
				set_param(3, result);
				ip += 4;
				break;
			}
			case 3: 
				set_param(1, input[input_idx++]);
				ip += 2;
				break;
			case 4:
				output.push_back(get_param(1));
				ip += 2;
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
	run_program(program, {1}, output);

	cout << "Output: " << output << endl;

	return 1;
}
