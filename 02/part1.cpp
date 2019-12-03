#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

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

void dump_program(const vector<int> &program) {
	string delim = "";

	for (auto v : program) {
		cout << delim << v;
		delim = ",";
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

	// restore to the "1202 program alarm" state
	program[1] = 12;
	program[2] = 2;

	run_program(program);
	dump_program(program);
}
