#include <iostream>
#include <vector>

using namespace std;

static constexpr int DECK_SIZE = 10007;
static constexpr int CARD = 2019;

inline int mod(int a, int b) {
	return (a % b + b) % b;
}

int main() {
	string shuffle_cmd;
	int a = 1;
	int b = 0;

	// the shuffle commands can be written as affine transformation in modular arithmetic
	// of the form pos(x) = (a * x + b) mod DECK_SIZE
	//  - reverse(x) = (DECK_SIZE - x - 1) mod DECK_SIZE
	//	- cut(x,n) = (x + DECK_SIZE - n) mod DECK_SIZE
	//	- inc(x,n) = (x * n) mod DECK_SIZE
	// we combine these transformations into a single transformation and apply it to the wanted card

	while(getline(cin, shuffle_cmd)) {
		if (shuffle_cmd.starts_with("deal with increment")) {
			int n = stoi(shuffle_cmd.substr(20));
			a = mod(n * a, DECK_SIZE);
			b = mod(n * b, DECK_SIZE);
		} else if (shuffle_cmd.starts_with("deal into new stack")) {
			a = mod(-a, DECK_SIZE);
			b = mod(-b + DECK_SIZE - 1, DECK_SIZE);
		} else if (shuffle_cmd.starts_with("cut")) {
			int n = stoi(shuffle_cmd.substr(4));
			a = mod(a, DECK_SIZE);
			b = mod(b + DECK_SIZE - n, DECK_SIZE);
		} else {
			cout << "Unknown command: " << shuffle_cmd << endl;
		}

	}

	cout << "Card " << CARD << " ends up at position " << mod(a * CARD + b, DECK_SIZE) << endl;

	return 0;
}
