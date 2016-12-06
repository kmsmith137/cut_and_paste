#include <sstream>
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

    template<unsigned int M> 
    inline float extract() const
    {
	// _mm_extract_ps() returns int instead of float
	union { int i; float x; } u;
	u.i = _mm_extract_ps(x, M);
	return u.x;
    }

    // Returns the horizontal sum of elements as a constant simd vector [ x ... x ].
    inline simd_t<float,4> horizontal_sum() const
    {
	__m128 y = x + _mm_shuffle_ps(x, x, 0xb1);   // (2301)_4 = 0xb1
	return y + _mm_shuffle_ps(y, y, 0x4e);       // (1032)_4 = 0x4e
    }

    // Returns the horizontal sum of elements as a scalar.
    inline float sum() const
    {
	simd_t<float,4> y = this->horizontal_sum();
	
	// For some reason calling extract<0>() here gives a compiler error, so we cut-and-paste.
	union { int i; float x; } u;
	u.i = _mm_extract_ps(y.x, 0);
	return u.x;
    }

    std::string str(bool bracket=true) const
    {
	std::stringstream ss;
	
	if (bracket)
	    ss << "[ ";

	union { int i; float x; } u;
	u.i = _mm_extract_ps(x,0); ss << u.x << ", ";
	u.i = _mm_extract_ps(x,1); ss << u.x << ", ";
	u.i = _mm_extract_ps(x,2); ss << u.x << ", ";
	u.i = _mm_extract_ps(x,3); ss << u.x;

	if (bracket)
	    ss << " ]";

	return ss.str();
    }
};


template<> struct simd_t<float,8>
{
    __m256 x;

    simd_t() { }
    simd_t(__m256 y) { x = y; }
    simd_t(float y)  { x = _mm256_set1_ps(y); }

    inline void set1(float y) { x = _mm256_set1_ps(y); }
    inline void setzero() { x = _mm256_setzero_ps(); }

    inline void load(const float *p)  { x = _mm256_load_ps(p); }
    inline void loadu(const float *p) { x = _mm256_loadu_ps(p); }

    inline void store(float *p) const  { _mm256_store_ps(p,x); }
    inline void storeu(float *p) const { _mm256_storeu_ps(p,x); }

    template<unsigned int M> 
    inline float extract() const
    {
	simd_t<float,4> x2 = _mm256_extractf128_ps(x, M/4);
	return x2.extract<M%4> ();
    }

    // Returns the horizontal sum of elements as a constant simd vector [ x ... x ].    
    inline simd_t<float,8> horizonal_sum() const
    {
	__m256 y = x + _mm256_shuffle_ps(x, x, 0xb1);   // (2301)_4 = 0xb1
	y += _mm256_shuffle_ps(y, y, 0x4e);             // (1032)_4 = 0x4e
	return y + _mm256_permute2f128_ps(y, y, 0x01);
    }

    // Returns the horizontal sum of elements as a scalar.
    inline float sum() const
    {
	__m256 y = x + _mm256_permute2f128_ps(x, x, 0x01);

	simd_t<float,4> z = _mm256_extractf128_ps(y, 0);
	return z.sum();
    }

    std::string str(bool bracket=true) const
    {
	simd_t<float,4> x0 = _mm256_extractf128_ps(x, 0);
	simd_t<float,4> x1 = _mm256_extractf128_ps(x, 1);

	std::stringstream ss;
	
	if (bracket)
	    ss << "[ ";

	ss << x0.str(false) << ", " << x1.str(false);

	if (bracket)
	    ss << " ]";

	return ss.str();
    }
};


template<typename T, unsigned int S> inline simd_t<T,S> &operator+=(simd_t<T,S> &a, simd_t<T,S> b) { a.x += b.x; return a; }
template<typename T, unsigned int S> inline simd_t<T,S> &operator-=(simd_t<T,S> &a, simd_t<T,S> b) { a.x -= b.x; return a; }
template<typename T, unsigned int S> inline simd_t<T,S> &operator*=(simd_t<T,S> &a, simd_t<T,S> b) { a.x *= b.x; return a; }
template<typename T, unsigned int S> inline simd_t<T,S> &operator/=(simd_t<T,S> &a, simd_t<T,S> b) { a.x /= b.x; return a; }

template<typename T, unsigned int S> inline simd_t<T,S> operator+(simd_t<T,S> a, simd_t<T,S> b) { return a.x + b.x; }
template<typename T, unsigned int S> inline simd_t<T,S> operator-(simd_t<T,S> a, simd_t<T,S> b) { return a.x - b.x; }
template<typename T, unsigned int S> inline simd_t<T,S> operator*(simd_t<T,S> a, simd_t<T,S> b) { return a.x * b.x; }
template<typename T, unsigned int S> inline simd_t<T,S> operator/(simd_t<T,S> a, simd_t<T,S> b) { return a.x / b.x; }

template<typename T, unsigned int S> inline simd_t<T,S> operator*(T a, simd_t<T,S> b) { return simd_t<T,S>(a) * b; }
template<typename T, unsigned int S> inline simd_t<T,S> operator*(simd_t<T,S> a, T b) { return a * simd_t<T,S>(b); }


#endif
