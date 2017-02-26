#include <cassert>
#include <iostream>

#include "lexical_cast.hpp"
#include "arithmetic_inlines.hpp"

using namespace std;


static void test_round_up_to_power_of_two()
{
    assert(round_up_to_power_of_two(1) == 1);

    for (ssize_t p = 0; p < 10; p++) {
	ssize_t n_lo = max((1<<p) - 10, (1<<p)/2 + 1);
	ssize_t n_hi = min((1<<p) + 10, (1<<(p+1)));

	for (ssize_t n = n_lo; n <= (1<<p); n++)
	    assert(round_up_to_power_of_two(n) == (1 << p));
	for (ssize_t n = (1<<p) + 1; n <= n_hi; n++)
	    assert(round_up_to_power_of_two(n) == (1 << (p+1)));
    }

    cout << "test_round_up_to_power_of_two: pass" << endl;
}


int main(int argc, char **argv)
{
    test_round_up_to_power_of_two();
    test_lexical_cast();
    return 0;
}
