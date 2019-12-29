#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <functional>
#include <algorithm>

#include <unistd.h>

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

int main() {
	// load program
	auto program = read_program_source(); 

	// run to check the tractor beam (just brute-force it)
	bool hor = true;
	int affected = 0;

	for (int y = 0; y < 50; ++y) {
		for (int x = 0; x < 50; ++x) {
			Computer comp(program);
			comp.run(
				[&]() -> base_t {		// input
					auto result = (hor) ? x : y;
					hor = !hor;
					return result;
				},
				[&](base_t output) {	// output
					const static char symbol[] = {'.', '#'};
					cout << symbol[output];
					affected += output;
				}
			);
		}

		cout << endl;
	}

	cout << "Affected points = " << affected << endl;
	return 0;
}
