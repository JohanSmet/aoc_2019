#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

struct node_t {
	node_t *			parent = nullptr;
	vector<node_t *>	children;
};

using map_t = unordered_map<string, node_t>;

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

int checksum_map(node_t *node, int sequence) {
	
	int result = sequence;
	for (auto child : node->children) {
		result += checksum_map(child, sequence + 1);
	}
	
	return result;
}

int main() {
	map_t map;

	if (!read_map(map)) {
		return -1;
	}

	auto com_iter = map.find("COM");
	if (com_iter == end(map)) {
		cout << "No CenterOfMass" << endl;
		return -1;
	}
	auto com = &com_iter->second;

	cout << checksum_map(com, 0) << endl;
	
	return 0;
}
