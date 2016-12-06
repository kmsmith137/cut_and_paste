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

    inline void horizontal_sum_in_place()
    {
	m.horizontal_sum_in_place();
	v.horizontal_sum_in_place();
    }

    // sum(): returns sum of all scalar elements in the matrix
    inline T sum() const { return this->vertical_sum().sum(); }

    inline T compare(const simd_trimatrix<T,S,N> &b) const
    {
	const simd_trimatrix<T,S,N> &a = *this;

	simd_trimatrix<T,S,N> t = (a-b);
	float num = (t*t).sum();
	
	t = a*a + b*b;
	float den = t.sum();
	
	return (den > 0.0) ? sqrt(num/den) : 0.0;
    }

    inline T compare(const T *p) const
    {
	simd_trimatrix<T,S,N> b;
	b.loadu(p);
	return this->compare(b);
    }

    // In-register linear algebra inlines start here.
    // Note: "in place" versions of multiply_*() and solve_*() would be easy to implement if needed.

    inline simd_ntuple<T,S,N> multiply_lower(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.x = this->v.vertical_dot(t);
	ret.v = this->m.multiply_lower(t.v);
	return ret;
    }

    inline simd_ntuple<T,S,N> multiply_upper(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.multiply_upper(t.v);
	ret.v += this->v.v * t.x;
	ret.x = this->v.x * t.x;
	return ret;
    }

    inline simd_ntuple<T,S,N> multiply_symmetric(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.multiply_symmetric(t.v) + this->v.v * t.x;
	ret.x = this->v.vertical_dot(t);
	return ret;
    }

    inline simd_ntuple<T,S,N> solve_lower(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.solve_lower(t.v);
	ret.x = this->v.v._vertical_dotn(ret.v, t.x) / this->v.x;
	return ret;
    }

    inline simd_ntuple<T,S,N> solve_upper(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.x = t.x / this->v.x;
	ret.v = this->m.solve_upper(t.v - this->v.v * ret.x);
	return ret;
    }

    inline void cholesky_in_place()
    {
	m.cholesky_in_place();
	v.v = m.solve_lower(v.v);
	
	simd_t<T,S> u = v.v._vertical_dotn(v.v, v.x);
	v.x = u.sqrt();
    }

    inline simd_trimatrix<T,S,N> cholesky()
    {
	simd_trimatrix<T,S,N> ret = *this;
	ret.cholesky_in_place();
	return ret;
    }

    inline void decholesky_in_place()
    {
	v.x = v.vertical_dot(v);
	v.v = m.multiply_lower(v.v);
	m.decholesky_in_place();
    }

    inline simd_trimatrix<T,S,N> decholesky()
    {
	simd_trimatrix<T,S,N> ret = *this;
	ret.decholesky_in_place();
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

    inline void horizontal_sum_in_place() { }
    inline void cholesky_in_place() { }
    inline void decholesky_in_place() { }
};


#endif // _SIMD_TRIMATRIX_HPP
