#include <bitset>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <vector>

// not a generic solution - assumes the shortest path in a quadrant is the best path overal e.g. when robot needs a key 
// from another quadrant it just waits at the door until a robot in another quadrant picks it up.

using namespace std;

using pos_t = array<int, 2>;
using keyset_t = bitset<26>;

struct Path {
	Path() = default;

	int			len = numeric_limits<int>::max();
	keyset_t	required_keys;
	keyset_t	collected_keys;
};

pos_t operator+(const pos_t &a, const pos_t &b) {
	return {a[0] + b[0], a[1] + b[1]};
}

struct Maze {

	bool load() {
		string line;
		int key_count = 0;
		int entrance_count = 0;
		keys.resize(26);

		while (getline(cin, line)) {
			for (int c = 0; c < line.size(); ++c) {
				if (line[c] >= 'a' && line[c] <= 'z') {
					keys[line[c]-'a'] = {c, static_cast<int> (maze.size())};
					key_count++;
				} else if (line[c] == '@') {
					entrance[entrance_count++] = {c, static_cast<int> (maze.size())};
				}
			}
			cout << line << endl;
			maze.push_back(line);
		}

		keys.resize(key_count);

		return true;
	}

	void compute_node_paths() {
		// initialize the data-structures
		key_paths.resize(keys.size(), vector<Path>(keys.size()));
		visited.resize(maze.size(), vector<bool>(maze.front().size(), false));
	
		// find the shortest path from the entrance to all the keys
		for (int r = 0; r < 4; ++r) {
			entrance_paths[r].resize(keys.size());
			r_walk_maze(entrance_paths[r], entrance[r], 0);
		}
		
		// find the shortest path between all the keys
		for (int i = 0; i < keys.size(); ++i) {
			for (auto &vrow : visited) {
				fill(begin(vrow), end(vrow), false);
			}

			r_walk_maze(key_paths[i], keys[i], 0);
		}

		// give each robot the keys which are unreachable for them
		for (int r = 0; r < 4; ++r) {
			for (int i = 0; i < keys.size(); ++i) {
				if (entrance_paths[r][i].len == numeric_limits<int>::max()) {
					robot_keys[r].set(i);
				}
			}
		}
	}

	void length_shortest_path(int robot, int node = -1, int total = 0, keyset_t retrieved = 0) {

		auto &paths = (node == -1) ? entrance_paths[robot] : key_paths[node];

		// at the end ?
		if (retrieved.count() == keys.size()) { 
			if (total < shortest_path[robot]) {
				shortest_path[robot] = total;
			}
			return;
		}

		// don't bother if the current path is already longer that the current shortest path
		if (total > shortest_path[robot]) {
			return;
		} 

		// check if we already know a shorter path to the current state (position + obtained keys)
		auto key = make_pair(retrieved.to_ulong(), node);
		auto found = path_cache.find(key);
		if (found != path_cache.end() && found->second < total) {
			return;
		}
		path_cache[key] = total;
		
		for (int next = 0; next < keys.size(); ++next) {

			// skip path if key already retrieved
			if (retrieved[next]) {
				continue;
			}

			// can we reach the key with our current keys?
			if ((retrieved & paths[next].required_keys) != paths[next].required_keys) {
				continue;
			}

			length_shortest_path(robot, next, total + paths[next].len, retrieved | paths[next].collected_keys);
		}
	}

private:
	bool is_valid_cell(pos_t cell) {
		if (cell[1] < 0 || cell[1] >= maze.size() ||
			cell[0] < 0 || cell[0] >= maze[cell[1]].size()) {
			return false;
		}
		char c = maze[cell[1]][cell[0]];
		return c != '#';
	}

	void r_walk_maze(vector<Path> &path, pos_t cur, int len, keyset_t required_keys = 0, keyset_t collected_keys = 0) {

		visited[cur[1]][cur[0]] = true;
		
		// check current cell
		char c = maze[cur[1]][cur[0]];
		if (c >= 'a' && c <= 'z') {				// found a key
			int k = c - 'a';
			collected_keys.set(k);
			if (len < path[k].len) {
				path[k].len = len;
				path[k].required_keys = required_keys;
				path[k].collected_keys = collected_keys;
			}
		} else if (c >= 'A' && c <= 'Z') {		// found a door
			int k = tolower(c) - 'a';
			required_keys.set(k);
		}

		// move to another cell
		static const pos_t possible_moves[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
		for (auto m : possible_moves) {
			auto next = cur + m;
			if (is_valid_cell(next) && !visited[next[1]][next[0]]) {
				r_walk_maze(path, next, len + 1, required_keys, collected_keys);
			}
		}

		visited[cur[1]][cur[0]] = false;

	}

public:
	vector<string>			maze;
	vector<pos_t>			keys;
	pos_t					entrance[4];
	vector<Path>			entrance_paths[4];
	vector<vector<Path>>	key_paths;
	vector<vector<bool>>	visited;
	int						shortest_path[4] = {
		numeric_limits<int>::max(),
		numeric_limits<int>::max(),
		numeric_limits<int>::max(),
		numeric_limits<int>::max()
	};
	keyset_t				robot_keys[4];
	map<pair<unsigned long, int>, int> path_cache;
};

int main() {
	Maze maze;

	if (!maze.load()) {
		return -1;
	}

	maze.compute_node_paths();
	
	for (int r = 0; r < 4; ++r) {
		maze.path_cache.clear();
		maze.length_shortest_path(r, -1, 0, maze.robot_keys[r]);
		cout << "Robot " << r << ": " << maze.shortest_path[r] << endl;
	}

	cout << "Total: " << std::accumulate(begin(maze.shortest_path), end(maze.shortest_path), 0) << endl;

	return 0;
}
