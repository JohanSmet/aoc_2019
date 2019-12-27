#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

vector<int> read_program_source() {

	vector<int> result;
	int v;
	char c;
	
	while (cin >> v) {
		result.push_back(v);
		cin >> c;		// read delimiter
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
	// load program
	auto program = read_program_source();

	// restore to the "1202 program alarm" state
	program[1] = 12;
	program[2] = 2;

	run_program(program);
	cout << program[0] << endl;
}
