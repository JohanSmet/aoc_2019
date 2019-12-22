#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <queue>

#include <unistd.h>

using namespace std;

enum PORTAL_TYPE {INNER = 0, OUTER = 1};

using Portal = array<int, 2>;

struct Node {
	int				id;
	vector<int>		neighbours;
	vector<int>		portals[2];
};

struct Maze {

	void load() {
		string line;
		int node_count = 0;

		// read input
		while (getline(cin, line)) {
			maze.push_back(line);

			// construct position to node index mapping
			vector<int> node_line;
			for (auto c : line) {
				node_line.push_back((c == '.') ? node_count++ : -1);
			}
			node_map.push_back(node_line);
		}

		// find portals
		auto is_portal = [](char c) {return c >= 'A' && c <= 'Z';};

		for (int r = 0; r < maze.size(); ++r) {
			for (int c = 0; c < maze[r].size(); ++c) {

				if (!is_portal(maze[r][c])) {
					continue;
				}
				string name = {maze[r][c]};
				int at_node = -1;
				PORTAL_TYPE type = INNER;

				if (is_portal(maze[r+1][c])) {
					name += maze[r+1][c];
					maze[r+1][c] = ' ';
					if (is_valid_pos(r+2,c)) {
						at_node = node_map[r+2][c];
						type = (r == 0) ? OUTER : INNER;
					} else {
						at_node = node_map[r-1][c];
						type = (r + 2) == node_map.size() ? OUTER : INNER;
					} 
				} else if (is_portal(maze[r][c+1])) {
					name += maze[r][c+1];
					maze[r][c+1] = ' ';
					if (is_valid_pos(r,c+2)) {
						at_node = node_map[r][c+2];
						type = (c == 0) ? OUTER : INNER;
					} else {
						at_node = node_map[r][c-1];
						type = (c + 2) == node_map[r].size() ? OUTER : INNER;
					}
				}
				
				if (name == "AA") {
					src = at_node;
				} else if (name == "ZZ") {
					dst = at_node;
				} else {
					portals[name][type] = at_node;
				}
			}
		}

		// construct graph
		nodes.resize(node_count);

		for (int r = 0; r < maze.size(); ++r) {
			for (int c = 0; c < maze[r].size(); ++c) {
				if (node_map[r][c] < 0) {
					continue;
				}
				Node &node = nodes[node_map[r][c]];
				node.id = node_map[r][c];

				auto add_neighbour = [&](int r, int c) {
					if (node_map[r][c] >= 0) {
						node.neighbours.push_back(node_map[r][c]);
					}
				};
				
				add_neighbour(r-1, c);
				add_neighbour(r+1, c);
				add_neighbour(r, c-1);
				add_neighbour(r, c+1);
			}
		}

		// apply portals
		for (auto portal : portals) {
			const auto &nb = portal.second;
			nodes[nb[INNER]].portals[INNER].push_back(nb[OUTER]);
			nodes[nb[OUTER]].portals[OUTER].push_back(nb[INNER]);
		}
	}

	bool is_valid_pos(int r, int c) {
		if (r < 0 || r >= node_map.size() ||
			c < 0 || c >= node_map[r].size()) {
			return false;
		}
		
		return node_map[r][c] >= 0;
	}

	int shortest_path(int src, int dst) {
		using ituple = tuple<int, int,int>;	// distance, level, node
		const int MAX_LEVELS = portals.size();

		priority_queue<ituple, vector<ituple>, greater<ituple>> pq;
		vector<vector<int>> dist(MAX_LEVELS, vector<int>(nodes.size(), numeric_limits<int>::max()));

		// start at src (distance == 0)
		pq.push(make_tuple(0, 0, src));
		dist[0][src] = 0;

		while (!pq.empty()) {
			auto [udist, level, u] = pq.top();
			pq.pop();

			if (level == 0 && u == dst) {
				return dist[level][u];
			}
			
			// loop through neighbours
			for (auto v : nodes[u].neighbours) {
				// is this a shorter path to v?
				if (dist[level][v] >= dist[level][u] + 1) {
					dist[level][v] = dist[level][u] + 1;
					pq.push(make_tuple(dist[level][v], level, v));
				}
			}

			// loop through active portals
			int pt_max = (level == 0) ? INNER : OUTER;
			static const int delta[] = {1, -1};

			for (int pt = 0; pt <= pt_max; ++pt) {
				int next_level = level + delta[pt];
				if (next_level >= MAX_LEVELS) {
					continue;
				}

				for (auto v : nodes[u].portals[pt]) {
					// is this a shorter path to v (on the level through the portal) ?
					if (dist[next_level][v] >= dist[level][u] + 1) {
						dist[next_level][v] = dist[level][u] + 1;
						pq.push(make_tuple(dist[next_level][v], next_level, v));
					} 
				} 
			}
		}

		return dist[0][dst];
	}

	vector<string>					maze;
	vector<vector<int>>				node_map;
	unordered_map<string, Portal>	portals;
	vector<Node>					nodes;
	int								src;
	int								dst;
};


int main() {
	// read maze
	Maze maze;
	maze.load();

	int shortest = maze.shortest_path(maze.src, maze.dst);
	cout << "Shortest path = " << shortest << endl; 

	return 0;
}
