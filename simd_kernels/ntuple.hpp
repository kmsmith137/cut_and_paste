#include "simd_ops.hpp"

#ifndef _NTUPLE_HPP
#define _NTUPLE_HPP


template<typename T, unsigned int N>
struct ntuple
{
    ntuple<T,N-1> v;
    T x;

    inline void setzero()
    {
	v.setzero();
	x = simd_ops<T>::setzero();
    }
};


template<typename T> 
struct ntuple<T,0> 
{ 
    inline void setzero() { }
};


#endif // _NTUPLE_HPP
