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

    // vertical_sum(): returns elementwise sum of all simd_t's in the triangular matrix
    inline simd_t<T,S> _vertical_sum(simd_t<T,S> x) const { return m._vertical_sum(x + v.vertical_sum()); }
    inline simd_t<T,S> vertical_sum() const { return m._vertical_sum(v.vertical_sum()); }

    // sum(): returns sum of all scalar elements in the matrix
    inline T sum() const { return this->vertical_sum().sum(); }

    inline T compare(const T *p) const
    {
	const simd_trimatrix<T,S,N> &a = *this;
	
	simd_trimatrix<T,S,N> b;
	b.loadu(p);

	simd_trimatrix<T,S,N> t = (a-b);
	float num = (t*t).sum();
	
	t = a*a + b*b;
	float den = t.sum();
	
	return (den > 0.0) ? sqrt(num/den) : 0.0;
    }

    // In-register linear algebra inlines start here.
    // Note: "in place" versions of multiply_*() and solve_*() would be easy to implement if needed.

    simd_ntuple<T,S,N> multiply_lower(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.x = this->v.vertical_dot(t);
	ret.v = this->m.multiply_lower(t.v);
	return ret;
    }

    simd_ntuple<T,S,N> multiply_upper(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.multiply_upper(t.v);
	ret.v += this->v.v * t.x;
	ret.x = this->v.x * t.x;
	return ret;
    }

    simd_ntuple<T,S,N> multiply_symmetric(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.multiply_symmetric(t.v) + this->v.v * t.x;
	ret.x = this->v.vertical_dot(t);
	return ret;
    }

    simd_ntuple<T,S,N> solve_lower(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.solve_lower(t.v);
	ret.x = this->v.v._vertical_dotn(ret.v, t.x) / this->v.x;
	return ret;
    }

    simd_ntuple<T,S,N> solve_upper(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.x = t.x / this->v.x;
	ret.v = this->m.solve_upper(t.v - this->v.v * ret.x);
	return ret;
    }
};


template<typename T, unsigned int S>
struct simd_trimatrix<T,S,0>
{
    inline void setzero() { }
    inline void loadu(const T *p) { }
    inline void storeu(T *p) const { }

    inline simd_trimatrix<T,S,0> &operator+=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator-=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator*=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator/=(const simd_trimatrix<T,S,0> &t) { return *this; }

    inline simd_trimatrix<T,S,0> operator+(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator-(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator*(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator/(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }

    inline simd_t<T,S> _vertical_sum(simd_t<T,S> x) const { return x; }

    inline simd_ntuple<T,S,0> multiply_lower(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> multiply_upper(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> multiply_symmetric(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }

    inline simd_ntuple<T,S,0> solve_lower(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> solve_upper(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
};


#endif // _SIMD_TRIMATRIX_HPP
