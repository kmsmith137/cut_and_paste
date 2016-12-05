#include "immintrin.h"

#ifndef _SIMD_T_HPP
#define _SIMD_T_HPP

template<typename T, unsigned int S> struct simd_t;

    
template<> struct simd_t<float,4>
{
    __m128 x;

    simd_t() { }
    simd_t(__m128 y) { x = y; }
    simd_t(float y)  { x = _mm_set1_ps(y); }

    inline void set1(float y) { x = _mm_set1_ps(y); }
    inline void setzero() { x = _mm_setzero_ps(); }

    inline void load(const float *p)  { x = _mm_load_ps(p); }
    inline void loadu(const float *p) { x = _mm_loadu_ps(p); }

    inline void store(float *p) const  { _mm_store_ps(p,x); }
    inline void storeu(float *p) const { _mm_storeu_ps(p,x); }
};


template<> struct simd_t<float,8>
{
    __m256 x;

    simd_t() { }
    simd_t(__m256 y) { x = y; }
    simd_t(float y)  { x = _mm256_set1_ps(y); }
    
    inline void setzero() { x = _mm256_setzero_ps(); }

    inline void load(const float *p)  { x = _mm256_load_ps(p); }
    inline void loadu(const float *p) { x = _mm256_loadu_ps(p); }

    inline void store(float *p) const  { _mm256_store_ps(p,x); }
    inline void storeu(float *p) const { _mm256_storeu_ps(p,x); }
};


template<typename T, unsigned int S> inline simd_t<T,S> operator+(simd_t<T,S> a, simd_t<T,S> b) { return a.x + b.x; }
template<typename T, unsigned int S> inline simd_t<T,S> operator-(simd_t<T,S> a, simd_t<T,S> b) { return a.x - b.x; }
template<typename T, unsigned int S> inline simd_t<T,S> operator*(simd_t<T,S> a, simd_t<T,S> b) { return a.x * b.x; }
template<typename T, unsigned int S> inline simd_t<T,S> operator/(simd_t<T,S> a, simd_t<T,S> b) { return a.x / b.x; }

template<typename T, unsigned int S> inline simd_t<T,S> operator*(T a, simd_t<T,S> b) { return simd_t<T,S>(a) * b; }
template<typename T, unsigned int S> inline simd_t<T,S> operator*(simd_t<T,S> a, T b) { return a * simd_t<T,S>(b); }


#endif
