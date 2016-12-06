#include "simd_t.hpp"
#include "simd_ntuple.hpp"

#ifndef _SIMD_TRIMATRIX_HPP
#define _SIMD_TRIMATRIX_HPP

template<typename T, unsigned int S, unsigned int N>
struct simd_trimatrix {
    simd_trimatrix<T,S,N-1> m;
    simd_ntuple<T,S,N> v;

    simd_trimatrix() { }
        
    inline void setzero()
    {
	m.setzero();
	v.setzero();
    }

    inline void loadu(const T *p)
    {
	m.loadu(p);
	v.loadu(p+(N*(N-1)*S)/2);
    }

    inline void storeu(T *p) const
    {
	m.storeu(p);
	v.storeu(p+(N*(N-1)*S)/2);
    }

    inline simd_trimatrix<T,S,N> &operator+=(const simd_trimatrix<T,S,N> &t)
    {
	m += t.m;
	v += t.v;
	return *this;
    }

    inline simd_trimatrix<T,S,N> &operator-=(const simd_trimatrix<T,S,N> &t)
    {
	m -= t.m;
	v -= t.v;
	return *this;
    }

    inline simd_trimatrix<T,S,N> &operator*=(const simd_trimatrix<T,S,N> &t)
    {
	m *= t.m;
	v *= t.v;
	return *this;
    }

    inline simd_trimatrix<T,S,N> &operator/=(const simd_trimatrix<T,S,N> &t)
    {
	m /= t.m;
	v /= t.v;
	return *this;
    }

    inline simd_trimatrix<T,S,N> operator+(const simd_trimatrix<T,S,N> &t) const
    {
	simd_trimatrix<T,S,N> ret;
	ret.m = m + t.m;
	ret.v = v + t.v;
	return ret;
    }

    inline simd_trimatrix<T,S,N> operator-(const simd_trimatrix<T,S,N> &t) const
    {
	simd_trimatrix<T,S,N> ret;
	ret.m = m - t.m;
	ret.v = v - t.v;
	return ret;
    }

    inline simd_trimatrix<T,S,N> operator*(const simd_trimatrix<T,S,N> &t) const
    {
	simd_trimatrix<T,S,N> ret;
	ret.m = m * t.m;
	ret.v = v * t.v;
	return ret;
    }

    inline simd_trimatrix<T,S,N> operator/(const simd_trimatrix<T,S,N> &t) const
    {
	simd_trimatrix<T,S,N> ret;
	ret.m = m / t.m;
	ret.v = v / t.v;
	return ret;
    }

    // only used in debug paths, so not optimized
    inline T sum() const { return m.sum() + v.sum(); }

    // only used in debug paths, so not optimized
    inline T compare(const T *p) const
    {
	const simd_trimatrix<T,S,N> &a = *this;
	
	simd_trimatrix<T,S,N> b;
	b.loadu(p);

	simd_trimatrix<T,S,N> t = (a-b) * (a-b);
	float num = t.sum();
	
	t = a*a + b*b;
	float den = t.sum();
	
	return (den > 0.0) ? sqrt(num/den) : 0.0;
    }
};


template<typename T, unsigned int S>
struct simd_trimatrix<T,S,0>
{
    inline void setzero() { }
    inline void loadu(const T *p) { }
    inline void storeu(T *p) { }

    inline simd_trimatrix<T,S,0> &operator+=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator-=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator*=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator/=(const simd_trimatrix<T,S,0> &t) { return *this; }

    inline simd_trimatrix<T,S,0> operator+(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator-(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator*(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator/(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }

    inline T sum() const { return 0.0; }
};


#endif // _SIMD_TRIMATRIX_HPP
