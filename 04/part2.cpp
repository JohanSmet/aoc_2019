#include <iostream>
#include <cassert>

using namespace std;
static const int RANGE_MIN = 240298;
static const int RANGE_MAX = 784956;

bool is_valid(int num) {
	assert(num >= 100000);
	assert(num <= 999999);

	auto double_digit = false;
	auto match_count = 0;
	auto prev_digit = num % 10;
	num /= 10;

	while (num > 0) {
		auto digit = num % 10;
		if (prev_digit < digit) {
			return false;
		}
		if (prev_digit != digit) {
			double_digit = double_digit || match_count == 1;
			match_count = 0;
		} else {
			++match_count;
		}

		num /= 10;
		prev_digit = digit;
	}
	double_digit = double_digit || match_count == 1;

	return double_digit;
}


int main()  {
	// cout << is_valid(111111) << endl;
	// cout << is_valid(223450) << endl;
	// cout << is_valid(123789) << endl;
	int count = 0;
	for (int num = RANGE_MIN; num <= RANGE_MAX; ++num) {
		count += is_valid(num);
	}

	cout << count << endl;

	return 0;
}
