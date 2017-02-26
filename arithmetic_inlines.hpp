inline ssize_t gcd(ssize_t m, ssize_t n)
{
    if (m < n)
	std::swap(m, n);
    if (n < 0)
        throw std::runtime_error("gcd() called with negative argument");

    while (n > 0) {
        ssize_t d = m % n;
        m = n;
        n = d;
    }

    return m;
}

inline ssize_t round_up_to_power_of_two(ssize_t n)
{
    if (n <= 0)
	throw std::runtime_error("round_up_to_power_of_two() called with non-positive argument");

    for (ssize_t i = 0; i < 64; i++) {
	if (n <= (1<<i))
	    return (1 << i);
    }

    throw std::runtime_error("overflow error in round_up_to_power_of_two()");
}
