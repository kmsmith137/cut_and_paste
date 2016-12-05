#include "immintrin.h"

#ifndef _SIMD_T_HPP
#define _SIMD_T_HPP

template<typename T, unsigned int S> struct simd_t;

    
template<> struct simd_t<float,4>
{
    __m128 x;

    inline void setzero() { x = _mm_setzero_ps(); }

    inline void load(const float *p)  { x = _mm_load_ps(p); }
    inline void loadu(const float *p) { x = _mm_loadu_ps(p); }

    inline void store(float *p)  { _mm_store_ps(p,x); }
    inline void storeu(float *p) { _mm_storeu_ps(p,x); }
};


template<> struct simd_t<float,8>
{
    __m256 x;
    
    inline void setzero() { x = _mm256_setzero_ps(); }

    inline void load(const float *p)  { x = _mm256_load_ps(p); }
    inline void loadu(const float *p) { x = _mm256_loadu_ps(p); }

    inline void store(float *p)  { _mm256_store_ps(p,x); }
    inline void storeu(float *p) { _mm256_storeu_ps(p,x); }
};


#endif
