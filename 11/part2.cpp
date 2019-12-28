#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>

using namespace std;

static const char *INPUT_FILE = "input.txt";

using base_t = int64_t;
using program_t = vector<base_t>;

class Computer {
public:
	// functions
	Computer(const program_t &prog) : program(prog) {
	}

	void add_input(initializer_list<base_t> in);
	void run_until_output();
	void run(vector<base_t> &output);

	// data
	program_t		program;
	base_t			ip = 0;			// instruction pointer
	base_t			bp = 0;			// relative base
	queue<base_t>	input;
	base_t			output = 0;
	bool			finished = false;
	bool			error = false;
};

program_t read_program_source() {

	program_t result;
	base_t v;
	char delim;
	
	while (cin >> v) {
		result.push_back(v);
		cin >> delim;
	}

	return result;
}

void Computer::add_input(initializer_list<base_t> in) {
	for (auto i : in) {
		input.push(i);
	}
}

void Computer::run_until_output() {
	auto opcode = [](int v) -> int {return v % 100;};

	auto check_memsize = [&](int min) {
		if (program.size() <= min) {
			program.resize(min + 1, 0);
		}
	};

	auto memory_ref = [&](base_t i) -> base_t & {
		assert(i >= 1 && i <= 3);
		static const int factor[] = {1, 100, 1000, 10000};
		auto mode = (program[ip] / factor[i]) % 10;
		base_t pos = 0;
		switch (mode) {
			case 0 :	// indirect
				pos = program[ip+i];
				break;
			case 1 :	// direct
				pos = ip+i;
				break;
			case 2 :	// relative
				pos = bp+program[ip+i];
				break;
			default:
				error = true;
				finished = true;
				cerr << "invalid parameter mode" << endl;
				return program[0];
		};
		check_memsize(pos);
		return program[pos];
	};

	auto get_data = [&](base_t i) -> base_t {
		auto result = memory_ref(i);
		return result;
	};

	auto set_data = [&](base_t i, base_t v) {
		auto &ref = memory_ref(i);
		ref = v;
	};

	while (ip < program.size()) {
		switch (opcode(program[ip])) {
			case 1: {		// addition
				auto result = get_data(1) + get_data(2);
				set_data(3, result);
				ip += 4;
				break;
			}
			case 2: {		// multiplication
				auto result = get_data(1) * get_data(2);
				set_data(3, result);
				ip += 4;
				break;
			}
			case 3:			// input
				set_data(1, input.front());
				input.pop();
				ip += 2;
				break;
			case 4:			// output
				output = get_data(1);
				ip += 2;
				return;
			case 5:	{		// jump-if-true
				auto cond = get_data(1);
				if (cond != 0) {
					ip = get_data(2);
				} else {
					ip += 3;
				}
				break;
			}
			case 6:	{		// jump-if-false
				auto cond = get_data(1);
				if (cond == 0) {
					ip = get_data(2);
				} else {
					ip += 3;
				}
				break;
			}
			case 7: 		// less than
				set_data(3, get_data(1) < get_data(2) ? 1 : 0);
				ip += 4;
				break;
			case 8: 		// equals
				set_data(3, get_data(1) == get_data(2) ? 1 : 0);
				ip += 4;
				break;
			case 9:			// set relative base
				bp += get_data(1);
				ip += 2;
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

void Computer::run(vector<base_t> &out) {
	if (!finished) {
		run_until_output();
	}

	while (!finished) {
		out.push_back(output);
		run_until_output();
	}
}

union Pos {
	struct {
		int32_t x;
		int32_t y;
	};
	uint64_t key;
};

int main() {
	// load program
	auto program = read_program_source(); 
	Computer comp(program);

	// run
	static const int move_delta[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
	static const int turn_delta[2] = {-1, 1};
	static const int black = 0;
	static const int white = 1;

	int direction = 0;	
	Pos position = {0, 0};

	unordered_map<uint64_t, int> painted_panels;
	painted_panels[position.key] = white;

	auto panel_color = [&](const Pos &pos) -> int {
		auto found = painted_panels.find(pos.key);
		return (found != end(painted_panels)) ? found->second : black;
	};

	while (!comp.finished) {
		// provide current color as input
		comp.add_input({panel_color(position)});

		// get the color to paint this panel
		comp.run_until_output();
		if (comp.finished) break;
		painted_panels[position.key] = comp.output;

		// get the turn direction 
		comp.run_until_output();
		if (comp.finished) break;
		
		// turn
		direction = (direction + turn_delta[comp.output] + 4) % 4;

		// move
		position.x += move_delta[direction][0];
		position.y += move_delta[direction][1];
	}

	cout << "Robot painted " << painted_panels.size() << " panels." << endl;

	// construct output image
	vector<vector<char>> output;
	for (auto iter: painted_panels) {
		Pos p; p.key = iter.first;
	
		if (p.y >= output.size()) {
			output.resize(p.y + 1);
		}

		if (p.x >= output[p.y].size()) {
			output[p.y].resize(p.x + 1, ' ');
		}

		output[p.y][p.x] = (iter.second == white) ? '#' : ' ';
	}

	for (const auto &row : output) {
		for (auto v : row) {
			cout << v;
		}
		cout << endl;
	}

	return 0;
}
