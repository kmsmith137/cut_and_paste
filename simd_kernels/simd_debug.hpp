#include <vector>
#include <random>
#include <cassert>
#include <iostream>
#include <type_traits>

#include "simd_t.hpp"
#include "simd_ntuple.hpp"
#include "simd_trimatrix.hpp"


// -------------------------------------------------------------------------------------------------
//
// vectorize(): unpacks a simd type to a std::vector


template<typename T, unsigned int S>
inline std::vector<T> vectorize(simd_t<T,S> x)
{
    std::vector<T> ret(S);
    x.storeu(&ret[0]);
    return ret;
}


template<typename T, unsigned int S, unsigned int N>
inline std::vector<T> vectorize(const simd_ntuple<T,S,N> &v)
{
    std::vector<T> ret(N*S);
    v.storeu(&ret[0]);
    return ret;
}


template<typename T, unsigned int S, unsigned int N>
inline std::vector<T> vectorize(const simd_trimatrix<T,S,N> &m)
{
    std::vector<T> ret((N*(N-1)*S)/2);
    m.storeu(&ret[0]);
    return ret;
}


// -------------------------------------------------------------------------------------------------
//
// Randomizers.
//
// Reminder: an RNG is initialized with
//
//   std::random_device rd;
//   std::mt19937 rng(rd());



// The helper function _uniform_randvec() has syntax
//    std::vector<T> _uniform_randvec(std::mt19937 &rng, unsigned int n, T lo, T hi);
// but is implemented differently for integral and floating-point types.

// Integral case: generate random number in range [lo,hi].  Note that endpoints are included in range.
template<typename T, typename std::enable_if<std::is_integral<T>::value,int>::type = 0>
inline std::vector<T> _uniform_randvec(std::mt19937 &rng, unsigned int n, T lo, T hi)
{
    std::vector<T> ret(n);
    for (unsigned int i = 0; i < n; i++)
	ret[i] = std::uniform_int_distribution<>(lo,hi)(rng);
    return ret;
}

// Floating-point case
template<typename T, typename std::enable_if<std::is_floating_point<T>::value,int>::type = 0>
inline std::vector<T> _uniform_randvec(std::mt19937 &rng, unsigned int n, T lo, T hi)
{
    std::vector<T> ret(n);
    for (unsigned int i = 0; i < n; i++)
	ret[i] = lo + (hi-lo) * std::uniform_real_distribution<>()(rng);
    return ret;
}


template<typename T, unsigned int S>
inline simd_t<T,S> uniform_random_simd_t(std::mt19937 &rng, T lo, T hi)
{
    std::vector<T> v = _uniform_randvec<T> (rng, S, lo, hi);
    return simd_t<T,S>::loadu(&v[0]);
}

template<typename T, unsigned int S, unsigned int N>
inline simd_ntuple<T,S,N> uniform_random_simd_ntuple(std::mt19937 &rng, T lo, T hi)
{
    std::vector<T> v = _uniform_randvec<T> (rng, N*S, lo, hi);
    simd_ntuple<T,S,N> ret;
    ret.loadu(&v[0]);
    return ret;
}


// -------------------------------------------------------------------------------------------------
//
// Print routines


template<class T, unsigned int S, unsigned int M>
struct _simd_writer 
{
    static inline void write(std::ostream &os, simd_t<T,S> x)
    {
	_simd_writer<T,S,M-1>::write(os, x);
	os << ", " << x.template extract<M-1>();
    }
};


template<class T, unsigned int S>
struct _simd_writer<T,S,1>
{
    static inline void write(std::ostream &os, simd_t<T,S> x) 
    { 
	os << x.template extract<0> ();
    }
};


template<typename T, unsigned int S>
std::ostream &operator<<(std::ostream &os, simd_t<T,S> x)
{
    os << "[";
    _simd_writer<T,S,S>::write(os, x);
    os << "]";
    return os;
}


// -------------------------------------------------------------------------------------------------
//
// Print routines, part 2


template<typename T, unsigned int S, unsigned int N>
struct _simd_nwriter
{
    static inline void write(std::ostream &os, const simd_ntuple<T,S,N> &v)
    {
	_simd_nwriter<T,S,N-1>::write(os, v.v);
	os << ", " << v.x;
    }

    static inline void write(std::ostream &os, const simd_trimatrix<T,S,N> &m)
    {
	_simd_nwriter<T,S,N-1>::write(os, m.m);
	os << ",\n " << m.v;
    }
};


template<typename T, unsigned int S>
struct _simd_nwriter<T,S,1>
{
    static inline void write(std::ostream &os, const simd_ntuple<T,S,1> &v)
    {
	os << v.x;
    }

    static inline void write(std::ostream &os, const simd_trimatrix<T,S,1> &m)
    {
	os << m.v;
    }
};


template<typename T, unsigned int S, unsigned int N>
std::ostream &operator<<(std::ostream &os, const simd_ntuple<T,S,N> &v)
{
    os << "{";
    _simd_nwriter<T,S,N>::write(os, v);
    os << "}";
    return os;
}


template<typename T, unsigned int S, unsigned int N>
std::ostream &operator<<(std::ostream &os, const simd_trimatrix<T,S,N> &m)
{
    os << "{";
    _simd_nwriter<T,S,N>::write(os, m);
    os << "}";
    return os;
}
