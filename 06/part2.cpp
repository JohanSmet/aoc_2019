#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

struct node_t {
	node_t *			parent = nullptr;
	vector<node_t *>	children;
	bool				visited = false;
};

using map_t = unordered_map<string, node_t>;

node_t *fetch_node(map_t &map, const string &name) {
	auto found = map.find(name);
	if (found != end(map)) {
		return &found->second;
	}

	return nullptr;
}

bool read_map(map_t &map) {
	string line;

	while (getline(cin, line)) {
		auto delim = line.find_first_of(')');
		auto center_name = line.substr(0, delim);
		auto object_name = line.substr(delim + 1);

		auto center = &map[center_name];
		auto object = &map[object_name];
		if (object->parent == nullptr) {
			object->parent = center;
			center->children.push_back(object);
		}
	}

	return true;
}

bool is_direct_child(node_t *node, node_t *target) {
	return count(begin(node->children), end(node->children), target) > 0;
};

int navigate(node_t *cur, node_t *target, int steps) {

	// end condition: we're at the node that has the target as a direct child 
	if (is_direct_child(cur, target)) {
		return steps;
	}

	cur->visited = true;

	// recurse to siblings
	for (auto sibling : cur->children) {
		if (!sibling->visited) {
			int result = navigate(sibling, target, steps + 1);
			if (result != 0) {
				return result;
			}
		}
	}

	// recurse to parent
	if (cur->parent && !cur->parent->visited) {
		return navigate(cur->parent, target, steps + 1);	
	}

	// not found in this subtree
	return 0;
}

int main() {
	map_t map;

	if (!read_map(map)) {
		return -1;
	}

	auto start = fetch_node(map, "YOU");
	auto target = fetch_node(map, "SAN");

	if (!start || !target) {
		cout << "invalid map" << endl;
	}

	start->visited = true;

	cout << "transfers: " << navigate(start->parent, target, 0) << endl;
	return 0;
}
