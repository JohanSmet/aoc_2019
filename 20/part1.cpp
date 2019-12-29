#include <array>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

using Portal = vector<int>;

struct Node {
	int				id;
	vector<int>		neighbours;
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

				if (is_portal(maze[r+1][c])) {
					name += maze[r+1][c];
					maze[r+1][c] = ' ';
					if (is_valid_pos(r+2,c)) {
						at_node = node_map[r+2][c];
					} else {
						at_node = node_map[r-1][c];
					} 
				} else if (is_portal(maze[r][c+1])) {
					name += maze[r][c+1];
					maze[r][c+1] = ' ';
					if (is_valid_pos(r,c+2)) {
						at_node = node_map[r][c+2];
					} else {
						at_node = node_map[r][c-1];
					}
				}
				portals[name].push_back(at_node); 
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
			if (nb.size() == 2) {
				nodes[nb[0]].neighbours.push_back(nb[1]);
				nodes[nb[1]].neighbours.push_back(nb[0]);
			}
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
		using ipair = pair<int,int>;

		priority_queue<ipair, vector<ipair>, greater<ipair>> pq;
		vector<int> dist(nodes.size(), numeric_limits<int>::max());

		// start at src (distance == 0)
		pq.push(make_pair(0, src));
		dist[src] = 0;

		while (!pq.empty()) {
			auto [udist, u] = pq.top();
			pq.pop();

			if (u == dst) {
				return dist[u];
			}
			
			// loop through neighbours
			for (auto v : nodes[u].neighbours) {
				// is this a shorter path to v?
				if (dist[v] > dist[u] + 1) {
					dist[v] = dist[u] + 1;
					pq.push(make_pair(dist[v], v));
				}
			}
		}

		return numeric_limits<int>::max();
	}

	vector<string>					maze;
	vector<vector<int>>				node_map;
	unordered_map<string, Portal>	portals;
	vector<Node>					nodes;
};


int main() {
	// read maze
	Maze maze;
	maze.load();

	int src = maze.portals["AA"].front();
	int dst = maze.portals["ZZ"].front();

	int shortest = maze.shortest_path(src, dst);
	cout << "Shortest path = " << shortest << endl;

	return 0;
}
