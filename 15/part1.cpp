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

enum Direction {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3
};

inline Direction invert_direction(Direction dir) {
	Direction result[] = {SOUTH, NORTH, EAST, WEST};
	return result[dir];
}

inline base_t direction_to_movement_command(Direction dir) {
	return dir + 1;
}

static constexpr int NODE_UNKNOWN = -1;
static constexpr int NODE_WALL = -2;
static constexpr int NODE_ROOT = -3;

// assumption: there are no loops in the maze (no need to check if moved into an already existing node)
//	 - seems to work for the puzzle input

struct Node {
	// interface functions
	Node () : parent(NODE_ROOT), dir_to_parent(SOUTH) {
		children = {NODE_UNKNOWN, NODE_UNKNOWN, NODE_UNKNOWN, NODE_UNKNOWN};
	}

	Node (int parent, Direction from_parent) : parent(parent) {
		dir_to_parent = invert_direction(from_parent);
		children = {NODE_UNKNOWN, NODE_UNKNOWN, NODE_UNKNOWN, NODE_UNKNOWN};
		children[dir_to_parent] = parent;
	}
	
	bool next_direction(Direction &dir) {
		auto found = find(begin(children), end(children), NODE_UNKNOWN);
		if (found == end(children)) {
			return false;
		}
		dir = static_cast<Direction>(found - begin(children));
		return true;
	}
	
	// data
	int parent;
	Direction dir_to_parent;
	array<int, 4> children;
};

struct Tree {
	Tree() {
		nodes.push_back(Node());
		current = 0;
	}

	bool next_move(base_t &cmd) {

		// move in the first available direction
		if (nodes[current].next_direction(checked_dir)) {
			cmd = direction_to_movement_command(checked_dir);
			backtracking = false;
			return true;
		}

		// backtrack to a node with an unexplored direction
		if (nodes[current].parent == NODE_ROOT) {
			return false;
		}

		checked_dir = nodes[current].dir_to_parent;
		cmd = direction_to_movement_command(checked_dir);
		current = nodes[current].parent;
		backtracking = true;
		return true;
	}

	void apply_output(base_t output) {
		if (output == 0) {
			assert(nodes[current].children[checked_dir] == NODE_UNKNOWN);
			nodes[current].children[checked_dir] = NODE_WALL;
			return;
		}
		
		if (!backtracking) {
			nodes[current].children[checked_dir] = nodes.size();
			nodes.push_back(Node(current, checked_dir));
			current = nodes.size() - 1;
		}

		if (output == 2) {
			oxigen_node = current;
		}
	}

	int r_shortest_path(int current, int steps) {
		auto &node = nodes[current];
		visited[current] = true;

		// check child nodes
		if (find(begin(node.children), end(node.children), oxigen_node) != end(node.children)) {
			return steps + 1;
		}

		// recurse
		int min_dist = numeric_limits<int>::max();
		for (auto child : node.children) {
			if (child > 0 && !visited[child]) {
				min_dist = min(min_dist, r_shortest_path(child, steps + 1));
			}
		}

		return min_dist;
	}

	int shortest_path() {
		visited.resize(nodes.size(), 0);
		fill(begin(visited), end(visited), false);

		return r_shortest_path(0, 0);
	}

	// data
	vector<Node> nodes;
	vector<bool> visited;
	int			 current;
	Direction	 checked_dir;
	bool		 backtracking = false;
	int			 oxigen_node = NODE_UNKNOWN;
};


int main() {
	// parse program
	auto program = read_program_source(); 
	Computer comp(program);

	// run
	Tree map;

	comp.run(
		[&]() -> base_t {
			base_t cmd = 1;
			if (!map.next_move(cmd)) {
				comp.finished = true;
			}
			return cmd;
		},
		[&](base_t output) {
			map.apply_output(output);
		}
	);

	int shortest = map.shortest_path();
	cout << "Shortest path to oxygen system = " << shortest << endl;

	return 0;
}
