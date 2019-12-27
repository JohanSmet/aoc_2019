#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

using bigint_t = __int128;

static constexpr bigint_t DECK_SIZE = 119315717514047;
static constexpr bigint_t REPEAT = 101741582076661;
static constexpr bigint_t POSITION = 2020;

inline bigint_t mod(bigint_t a, bigint_t b) {
	return (a % b + b) % b;
}

static bigint_t gcd_extended(bigint_t a, bigint_t b, bigint_t& x, bigint_t& y) {
	if (a == 0) {
		x = 0;
		y = 1;
		return b;
	}

	bigint_t x1, y1;
	bigint_t gcd = gcd_extended(b % a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return gcd;
}

inline bigint_t invmod(bigint_t a, bigint_t m) {
	bigint_t x,y;
	
	auto g = gcd_extended(a, m, x, y);
	return (g != 1) ? -1 : mod(x, m);
}

inline bigint_t powermod(bigint_t x, bigint_t y, bigint_t m) {
	bigint_t result = 1;

	x = mod(x, m);

	while (y > 0) {
		if (y & 1) {
			result = mod(result * x, m);
		}

		y >>= 1;
		x = mod(x * x, m);
	}
	
	return result;
}

int main() {
	// the shuffle commands can be written as affine transformation in modular arithmetic
	// of the form pos(x) = (a * x + b) mod DECK_SIZE
	//  - reverse(x) = (DECK_SIZE - x - 1) mod DECK_SIZE
	//	- cut(x,n) = (x + DECK_SIZE - n) mod DECK_SIZE
	//	- inc(x,n) = (x * n) mod DECK_SIZE
	// we combine these transformations into a single transformation and apply it to the wanted card

	string shuffle_cmd;

	bigint_t a = 1;
	bigint_t b = 0;

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

	// this function needs to be applied multiple times
	// f(f(x)) = f(ax + b) = a(ax + b) + b = a²x + ab + b
	// f(f(f(x))) = a(a²x + ab + b) + b = a³x + a²b + ab + b
	//  => f^k(x) = a^k * x + (a^k-1 + a^k-2 + ... + a + 1)b 
	//  => f^k(x) = a^k * x + ((a^k - 1) / (a - 1)) * b

	// apply repeat
	auto repeated_a = powermod(a, REPEAT, DECK_SIZE);
	auto repeated_b = mod(b * mod((repeated_a - 1) * invmod(a - 1, DECK_SIZE), DECK_SIZE), DECK_SIZE);

	// the constructed formula gives the position given a card after the shuffle x' = ax + b
	// we need the card given a position so we rearrange the formula to be x = (x' - b) / a
	int64_t card = mod(mod(POSITION - repeated_b, DECK_SIZE) * invmod(repeated_a, DECK_SIZE), DECK_SIZE);
	cout << card << endl;

	return 0;
}
