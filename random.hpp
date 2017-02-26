#include <random>
#include <vector>

// Reminder: an RNG is initialized with
//
//   std::random_device rd;
//   std::mt19937 rng(rd());

#ifndef _RNG_HPP
#define _RNG_HPP


// -------------------------------------------------------------------------------------------------
//
// Uniform distribution


inline double uniform_rand(std::mt19937 &rng)
{
    return std::uniform_real_distribution<>()(rng);
}

inline double uniform_rand(std::mt19937 &rng, double lo, double hi)
{
    return lo + (hi-lo) * uniform_rand(rng);
}

template<typename T> inline void uniform_rand(std::mt19937 &rng, T *dst, ssize_t n, double lo, double hi)
{
    for (ssize_t i = 0; i < n; i++)
	dst[i] = lo + (hi-lo) * uniform_rand(rng);
}

template<typename T> inline void uniform_rand(std::mt19937 &rng, T *dst, ssize_t n)
{
    uniform_rand(rng, dst, n, 0.0, 1.0);
}

template<typename T> inline void uniform_rand(std::mt19937 &rng, std::vector<T> &dst, double lo, double hi)
{
    uniform_rand(rng, &dst[0], dst.size(), lo, hi);
}

template<typename T> inline void uniform_rand(std::mt19937 &rng, std::vector<T> &dst)
{
    uniform_rand(rng, &dst[0], dst.size());
}

template<typename T> inline std::vector<T> uniform_randvec(std::mt19937 &rng, ssize_t n, double lo, double hi)
{
    std::vector<T> ret(n);
    uniform_rand(rng, &ret[0], n, lo, hi);
    return ret;
}

template<typename T> inline std::vector<T> uniform_randvec(std::mt19937 &rng, ssize_t n)
{
    return uniform_randvec<T> (rng, n, 0.0, 1.0);
}


// -------------------------------------------------------------------------------------------------
//
// Gaussian distribution


inline double gaussian_rand(std::mt19937 &rng, double rms=1.0)
{
    std::normal_distribution<> dist;
    return rms * dist(rng);
}

template<typename T> inline void gaussian_rand(std::mt19937 &rng, T *dst, ssize_t n, double rms=1.0)
{
    std::normal_distribution<> dist;
    for (ssize_t i = 0; i < n; i++)
	dst[i] = rms * dist(rng);
}

template<typename T> inline void gaussian_rand(std::mt19937 &rng, std::vector<T> &dst, double rms=1.0)
{
    gaussian_rand(rng, &dst[0], dst.size(), rms);
}

template<typename T> inline std::vector<T> gaussian_randvec(std::mt19937 &rng, ssize_t n, double rms=1.0)
{
    std::vector<T> ret(n);
    gaussian_rand(rng, &ret[0], n, rms);
    return ret;
}


// -------------------------------------------------------------------------------------------------
//
// Integer distributions


inline int randint(std::mt19937 &rng, int lo, int hi)
{
    return std::uniform_int_distribution<>(lo,hi-1)(rng);   // note hi-1 here!
}


// -------------------------------------------------------------------------------------------------
//
// Misc


template<typename T> inline void randomly_permute(std::mt19937 &rng, std::vector<T> &v)
{
    for (unsigned int i = 1; i < v.size(); i++) {
	int j = randint(rng, 0,i+1);
	std::swap(v[i], v[j]);
    }
}


#endif
