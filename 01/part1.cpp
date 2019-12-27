#include <iostream>

using namespace std;

int main() {
	int mass;
	int total_fuel = 0;

	while (cin >> mass) {
		total_fuel += (mass / 3) - 2;
	}

	cout << "Total fuel required = " << total_fuel << endl;

	return 0;
}
