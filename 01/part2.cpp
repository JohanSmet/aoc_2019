#include <iostream>

using namespace std;

int main() {
	int mass;
	int total_fuel = 0;

	auto fuel_needed = [](int x) -> int {
		return (x / 3) - 2;
	};

	while (cin >> mass) {
		int fuel = fuel_needed(mass);

		while (fuel > 0) {
			total_fuel += fuel;
			fuel = fuel_needed(fuel);
		}	
	}

	cout << "Total fuel required = " << total_fuel << endl;

	return 0;
}
