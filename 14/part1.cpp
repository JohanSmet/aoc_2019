#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

static const int MATERIAL_ORE = 0;
static const int MATERIAL_FUEL = 1;

struct Reaction {
	vector<int>	input_mat;
	vector<int> input_qua;

	int output_mat;
	int output_qua;
};

struct NanoFactory {

	bool read_configuation();
	int create_or_fetch_material(const string &name);
	int solve();
	
	vector<string>		materials;
	vector<Reaction>	reactions;
};


bool NanoFactory::read_configuation() {

	materials.push_back("ORE");
	materials.push_back("FUEL");

	string line;

	while (getline(cin, line)) {
		istringstream ss(line);
		int qua;
		string mat, sep;
		bool cont = true;

		Reaction reaction;

		while (cont) {
			ss >> qua >> mat;
			if (mat.back() == ',') {
				mat.pop_back();
			} else {
				cont = false;
			}

			reaction.input_mat.push_back(create_or_fetch_material(mat));
			reaction.input_qua.push_back(qua);
		}
		
		ss >> sep >> reaction.output_qua >> mat;
		reaction.output_mat = create_or_fetch_material(mat);

		if (reaction.output_mat >= reactions.size()) {
			reactions.resize(reaction.output_mat+1);
		}

		reactions[reaction.output_mat] = reaction; 
	}

	return true;
}

int NanoFactory::create_or_fetch_material(const string &name) {
	auto found = find(begin(materials), end(materials), name);
	if (found != end(materials)) {
		return found - begin(materials);
	} else {
		materials.push_back(name);
		return materials.size() - 1;
	}
}

int NanoFactory::solve() {
	vector<int> formula(materials.size(), 0);

	for (int i = 0; i < reactions[MATERIAL_FUEL].input_mat.size(); ++i) {
		formula[reactions[MATERIAL_FUEL].input_mat[i]] = reactions[MATERIAL_FUEL].input_qua[i];
	}

	while (true) {
		auto found = find_if(begin(formula)+2, end(formula), [](auto x) {return x > 0;});
		if (found == end(formula))
			break;
		int mat = found - begin(formula);
		
		int factor = ceil(static_cast<float> (formula[mat]) / reactions[mat].output_qua);

		formula[mat] -= factor * reactions[mat].output_qua;
		for (int i = 0; i < reactions[mat].input_mat.size(); ++i) {
			formula[reactions[mat].input_mat[i]] += factor * reactions[mat].input_qua[i];
		}
	}

	return formula[0];
}

int main() {
	NanoFactory factory;

	if (!factory.read_configuation()) {
		return -1;
	}

	int ore = factory.solve();
	cout << "Need " << ore << " ORE" << endl;

	return 0;
}
