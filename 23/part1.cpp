#include <cassert>
#include <deque>
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

	void step(input_callback_t input_func, output_callback_t output_func);
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

void Computer::step(input_callback_t input_func, output_callback_t output_func) {
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
				cout << "invalid parameter mode" << endl;
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

	if (!finished && ip < program.size()) {
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
				break;
			default:
				cout << "Invalid opcode: " << opcode(program[ip]) << " at ip = " << ip << endl;
				finished = true;
				error = true;
				break;
		}
	}
}

void Computer::run(input_callback_t input_func, output_callback_t output_func) {

	while (!finished && ip < program.size()) {
		step(input_func, output_func);
	}
}

int main() {
	// load program
	auto program = read_program_source(); 

	// setup computers
	static const int NETWORK_SIZE = 50;
	auto comps = vector<Computer>(NETWORK_SIZE, Computer(program));
	auto input_queue = vector<deque<base_t>>(NETWORK_SIZE);
	auto output_queue = vector<vector<base_t>>(NETWORK_SIZE);

	for (int addr = 0; addr < NETWORK_SIZE; ++addr) {
		input_queue[addr].push_back(addr);
	}

	bool done = false;

	while (!done) {
		
		for (int comp_idx = 0; !done && comp_idx < NETWORK_SIZE; ++comp_idx) {
			comps[comp_idx].step(
				[&]() -> base_t {		// input
					if (input_queue[comp_idx].empty()) {
						return -1;
					} else {
						base_t result = input_queue[comp_idx].front();
						input_queue[comp_idx].pop_front();
						return result;
					}
				},
				[&](base_t output) {	// output
					auto &q = output_queue[comp_idx];
					q.push_back(output);
					if (q.size() == 3) {
						if (q[0] == 255) {
							cout << "First packet sent to 255 = (" << q[1] << "," << q[2] << ")" << endl;
							done = true;
						} else {
							input_queue[q[0]].push_back(q[1]);
							input_queue[q[0]].push_back(q[2]);
						}
						
						q.clear();
					}
				}
			); 
		}
	}

	return 0;
}
