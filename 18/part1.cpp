#include <algorithm>
#include <bitset>
#include <iostream>
#include <map>
#include <vector>

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

ostream &operator<<(ostream &os, const keyset_t &keys) {
	for (int i = 0; i < keys.size(); ++i) {
		cout << ((keys[i]) ? (char)('a' + i) : '-');
	}
	return os;
}

struct Maze {

	bool load() {
		string line;
		int key_count = 0;
		keys.resize(26);

		while (getline(cin, line)) {
			for (int c = 0; c < line.size(); ++c) {
				if (line[c] >= 'a' && line[c] <= 'z') {
					keys[line[c]-'a'] = {c, static_cast<int> (maze.size())};
					key_count++;
				} else if (line[c] == '@') {
					entrance = {c, static_cast<int> (maze.size())};
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
		entrance_paths.resize(keys.size());
		key_paths.resize(keys.size(), vector<Path>(keys.size()));
		visited.resize(maze.size(), vector<bool>(maze.front().size(), false));
	
		// find the shortest path from the entrance to all the keys
		r_walk_maze(entrance_paths, entrance, 0);
		
		// find the shortest path between all the keys
		for (int i = 0; i < keys.size(); ++i) {
			for (auto &vrow : visited) {
				fill(begin(vrow), end(vrow), false);
			}

			r_walk_maze(key_paths[i], keys[i], 0);
		}
	}


	void length_shortest_path(int node = -1, int total = 0, keyset_t retrieved = 0) {

		auto &paths = (node == -1) ? entrance_paths : key_paths[node];

		// at the end ?
		if (retrieved.count() == keys.size()) { 
			if (total < shortest_path) {
				shortest_path = total;
			}
			return;
		}

		// don't bother if the current path is already longer that the current shortest path
		if (total > shortest_path) {
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

			length_shortest_path(next, total + paths[next].len, retrieved | paths[next].collected_keys);
		}
	}

	void dump_paths() {

		for (int i = 0; i < keys.size(); ++i) {
			cout << "entrance to " << (char) ('a' + i)
				 << " len:" << entrance_paths[i].len 
				 << " required keys:" << entrance_paths[i].required_keys
				 << " collected keys: " << entrance_paths[i].collected_keys
				 << endl;
		}

		for (int i = 0; i < keys.size(); ++i) {
			for (int j = 0; j < keys.size(); ++j) {
				cout << (char) ('a' + i) << " to " << (char) ('a' + j)
					 << " len:" << key_paths[i][j].len 
					 << " required keys:" << key_paths[i][j].required_keys
					 << " collected keys: " << key_paths[i][j].collected_keys
					 << endl;
			}
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
	pos_t					entrance;
	vector<Path>			entrance_paths;
	vector<vector<Path>>	key_paths;
	vector<vector<bool>>	visited;
	int						shortest_path	= numeric_limits<int>::max();
	map<pair<unsigned long, int>, int> path_cache;
};

int main() {
	Maze maze;

	if (!maze.load()) {
		return -1;
	}

	maze.compute_node_paths();
	// maze.dump_paths();

	maze.length_shortest_path();
	cout << maze.shortest_path << endl;

	return 0;
}
