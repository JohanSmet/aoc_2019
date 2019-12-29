#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

using base_t = int64_t;
using program_t = vector<base_t>;

using input_callback_t = function<base_t()>;
using output_callback_t = function<void(base_t)>;

class Computer {
public:
	// functions
	Computer(const program_t &prog) : program(prog) {
	}

	void run(input_callback_t input_func, output_callback_t output_func);

	// data
	program_t		program;
	base_t			ip = 0;			// instruction pointer
	base_t			bp = 0;			// relative base
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

void Computer::run(input_callback_t input_func, output_callback_t output_func) {
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

	while (!finished && ip < program.size()) {
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
			case 3: {		// input
				auto val = input_func();
				set_data(1, val);
				ip += 2;
				break;
			}
			case 4:			// output
				output_func(get_data(1));
				ip += 2;
				break;
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

enum Direction {UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3};
using pos_t = array<int, 2>;

Direction dir_from_output(char out) {
	switch (out) {
		case '^':
			return UP;
		case '>':
			return RIGHT;
		case 'v':
			return DOWN;
		case '<':
			return LEFT;
		default:
			return UP;
	}
}

pos_t operator+(const pos_t &p1, const pos_t &p2) {
	return {p1[0]+p2[0], p1[1]+p2[1]};
}

ostream &operator<<(ostream &os, const pos_t &pos) {
	cout << "<" << pos[0] << "," << pos[1] << ">";
	return os;
}

int main() {
	// parse program
	auto program = read_program_source(); 
	Computer comp(program);

	// run to get the maze structure
	vector<vector<int>> maze(1);
	pos_t robot;
	Direction robot_dir = LEFT;

	comp.run(
		[&]() -> base_t {		// input
			return 0;
		},
		[&](base_t output) {	// output
			switch (output) {
				case 46:		// empty
					maze.back().push_back(0);
					break;
				case 10:		// new-line
					maze.push_back(vector<int>());
					break;
				case 35:		// scaffold
					maze.back().push_back(1);
					break;
				default :
					maze.back().push_back(1);
					robot[0] = maze.back().size()-1;
					robot[1] = maze.size()-1;
					robot_dir = dir_from_output(output);
					break;
			}
		}
	);

	// remove last, empty, line
	maze.pop_back();

	// construct path
	string path;

	static const pos_t delta[4] = {{0,-1},{1,0},{0,1},{-1,0}};

	auto is_wall_at = [&](const pos_t &p) -> bool {
		if (p[1] >= 0 && p[1] < maze.size() &&
			p[0] >= 0 && p[0] < maze[p[1]].size()) {
			return maze[p[1]][p[0]] == 1;
		} else {
			return false;
		}
	};

	auto turn = [&](Direction dir, int rorl) -> Direction {
		return static_cast<Direction>((dir + 4 + rorl) % 4);
	};

	while (true) {
		int len = 0;

		// move as far as we can in the current direction
		while (is_wall_at(robot + delta[robot_dir])) {
			len += 1;
			robot = robot + delta[robot_dir];
		}

		if (len > 0) {
			path.append(to_string(len));
			path.append(",");
		}

		// rotate right or left or end reached 
		if (is_wall_at(robot + delta[turn(robot_dir, 1)])) {
			path.append("R,");
			robot_dir = turn(robot_dir, 1);
		} else if (is_wall_at(robot + delta[turn(robot_dir, -1)])) {
			path.append("L,");
			robot_dir = turn(robot_dir, -1);
		} else {
			break;
		}
	}

	// split out repeating sub-commands (max 3) and construct the main routine
	// I did this by hand to finish the assignment in the time available to me today
	string solution = 
		"A,B,A,C,A,B,C,B,C,B\n"		// main routine
		"R,8,L,10,L,12,R,4\n"		// function A
		"R,8,L,12,R,4,R,4\n"		// function B
		"R,8,L,10,R,8\n"			// function C
		"n\n";						// output (y/n)
	
	auto solution_iter = begin(solution);
	int dust = 0;

	Computer comp2(program);
	comp2.program[0] = 2;

	// run to get the maze structure
	comp2.run(
		[&]() -> base_t {		// input
			char c = *solution_iter;
			++solution_iter;
			cout << c;
			return c;
		},
		[&](base_t output) {	// output
			if (output > 255) {
				dust = output;
			} else {
				cout << (char) output;
			}
		}
	);

	cout << "Collected dust = " << dust << endl;
	return 0;
}
