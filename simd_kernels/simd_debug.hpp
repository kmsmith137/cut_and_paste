#include <iostream>
#include "simd_t.hpp"
#include "simd_ntuple.hpp"
#include "simd_trimatrix.hpp"


// -------------------------------------------------------------------------------------------------


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
