#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> read_program_source() {

	vector<int> result;
	int v;
	char c;
	
	while (cin >> v) {
		result.push_back(v);
		cin >> c;			// read delimiter
	}

	return result;
}

int run_program(vector<int> &program) {
	int pc = 0;

	while (pc < program.size()) {
		switch (program[pc]) {
			case 1: {
				int result = program[program[pc+1]] + program[program[pc+2]];
				program[program[pc+3]] = result;
				pc += 4;
				break;
			}
			case 2: {
				int result = program[program[pc+1]] * program[program[pc+2]];
				program[program[pc+3]] = result;
				pc += 4;
				break;
			}
			case 99: return 0;
			default:
				cerr << "Invalid opcode: " << program[pc] << endl;
				return -1;
		}
	}

	return -2;
}

int main() {

	auto clean_program = read_program_source();

	for (int noun = 0; noun <= 99; ++noun) {
		for (int verb = 0; verb <= 99; ++verb) {
			auto program = clean_program;
			program[1] = noun;
			program[2] = verb;
			run_program(program);
				
			if (program[0] == 19690720) {
				cout << (100 * noun) + verb << endl;
				return 0;
			}
		}
	}

	return 1;
}
