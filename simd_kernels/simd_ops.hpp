#include "immintrin.h"

#ifndef _SIMD_OPS_HPP
#define _SIMD_OPS_HPP

// FIXME assumes single-precision floating-point simd type

template<typename T>
struct simd_ops {
    static inline T setzero();

    static inline T load(const float *p);
    static inline T loadu(const float *p);

    static inline void store(float *p, T x);
    static inline void storeu(float *p, T x);
};


template<> inline __m128 simd_ops<__m128>::setzero()  { return _mm_setzero_ps(); }
template<> inline __m128 simd_ops<__m128>::load(const float *p)  { return _mm_load_ps(p); }
template<> inline __m128 simd_ops<__m128>::loadu(const float *p)  { return _mm_loadu_ps(p); }
template<> inline void simd_ops<__m128>::store(float *p, __m128 x)  { _mm_store_ps(p, x); }
template<> inline void simd_ops<__m128>::storeu(float *p, __m128 x)  { _mm_storeu_ps(p, x); }

template<> inline __m256 simd_ops<__m256>::setzero()  { return _mm256_setzero_ps(); }
template<> inline __m256 simd_ops<__m256>::load(const float *p)  { return _mm256_load_ps(p); }
template<> inline __m256 simd_ops<__m256>::loadu(const float *p)  { return _mm256_loadu_ps(p); }
template<> inline void simd_ops<__m256>::store(float *p, __m256 x)  { _mm256_store_ps(p, x); }
template<> inline void simd_ops<__m256>::storeu(float *p, __m256 x)  { _mm256_storeu_ps(p, x); }


#endif
