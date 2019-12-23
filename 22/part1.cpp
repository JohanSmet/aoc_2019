#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

static constexpr int DECK_SIZE = 10007;

inline void dump_vector(const char *label, const vector<int> &v) {
	cout << label;
	for (auto i : v) {
		cout << " " << i;
	}
	cout << endl;
}

vector<int> deal_with_increment(const vector<int> &deck, int inc) {
	cout << "deal with increment = " << inc << endl;
	
	vector<int> result(deck.size(), -1);
	int src = 0;
	int dst = 0;

	while (src < deck.size()) {
		result[dst] = deck[src++];
		dst = (dst + inc) % deck.size();
	}

	return result;
}

vector<int> deal_into_new_stack(const vector<int> &deck) {
	cout << "deal into new stack" << endl;

	vector<int> result;
	result.reserve(deck.size());
	reverse_copy(begin(deck), end(deck), back_inserter(result));
	return result;
}

vector<int> cut(const vector<int> &deck, int n) {
	cout << "cut = " << n << endl;

	vector<int> result;
	result.reserve(deck.size());

	if (n > 0) {
		copy(begin(deck)+n,end(deck), back_inserter(result));
		copy_n(begin(deck), n, back_inserter(result));
	} else {
		copy(begin(deck) + deck.size() + n, end(deck), back_inserter(result));
		copy_n(begin(deck), deck.size() + n, back_inserter(result));
	}

	return result;
}

int main() {
	vector<int>	deck(DECK_SIZE);
	iota(begin(deck), end(deck), 0);

	string shuffle_cmd;

	while(getline(cin, shuffle_cmd)) {
		if (shuffle_cmd.starts_with("deal with increment")) {
			deck = deal_with_increment(deck, stoi(shuffle_cmd.substr(20)));
		} else if (shuffle_cmd.starts_with("deal into new stack")) {
			deck = deal_into_new_stack(deck);
		} else if (shuffle_cmd.starts_with("cut")) {
			deck = cut(deck, stoi(shuffle_cmd.substr(4)));
		} else {
			cout << "Unknown command: " << shuffle_cmd << endl;
		}
	}

	auto found = find(begin(deck), end(deck), 2019);
	cout << "Card 2019 is at position " << found - begin(deck) << endl;


	return 0;
}
