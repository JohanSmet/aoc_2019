#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

namespace {
	static const char *INPUT_FILE = "input.txt";
}

vector<int> parse_program_source(const string &src) {

	stringstream ss(src);

	vector<int> result;
	int v;
	char c;
	
	while (ss >> v) {
		result.push_back(v);
		ss >> c;
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

	// run program 
	vector<int> output;
	run_program(program, {1}, output);
	dump_vector("output:", output);

	return 1;
}
