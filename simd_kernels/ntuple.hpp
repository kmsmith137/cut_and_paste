#include "simd_ops.hpp"

#ifndef _NTUPLE_HPP
#define _NTUPLE_HPP


template<typename T, unsigned int S, unsigned int N>
struct simd_ntuple
{
    simd_ntuple<T,S,N-1> v;
    simd_t<T,S> x;

    inline void setzero()
    {
	v.setzero();
	x.setzero();
    }

    inline void loadu(const float *p)
    {
	v.loadu(p);
	x.loadu(p+(N-1)*S);
    }
};


template<typename T, unsigned int S> 
struct simd_ntuple<T,S,0> 
{ 
    inline void setzero() { }
    inline void loadu(const float *p) { }
};


#endif // _NTUPLE_HPP
