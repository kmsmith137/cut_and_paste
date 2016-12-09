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

    inline void set_identity()
    {
	m.set_identity();
	v.v.setzero();
	v.x = 1;
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

    inline void set1_slow(const T *p)
    {
	m.set1_slow(p);
	v.set1_slow(p + (N*(N-1))/2);
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

    inline simd_ntuple<T,S,N> multiply_symmetric(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = this->m.multiply_symmetric(t.v) + this->v.v * t.x;
	ret.x = this->v.vertical_dot(t);
	return ret;
    }

    inline void multiply_lower_in_place(simd_ntuple<T,S,N> &t) const
    {
	t.x = this->v.vertical_dot(t);
	this->m.multiply_lower_in_place(t.v);
    }

    inline void multiply_upper_in_place(simd_ntuple<T,S,N> &t) const
    {
	this->m.multiply_upper_in_place(t.v);
	t.v += this->v.v * t.x;
	t.x *= this->v.x;
    }

    inline void solve_lower_in_place(simd_ntuple<T,S,N> &t) const
    {
	this->m.solve_lower_in_place(t.v);
	t.x = this->v.v._vertical_dotn(t.v, t.x) / this->v.x;
    }

    inline void solve_upper_in_place(simd_ntuple<T,S,N> &t) const
    {
	t.x /= this->v.x;
	t.v -= this->v.v * t.x;
	this->m.solve_upper_in_place(t.v);
    }

    inline void cholesky_in_place()
    {
	m.cholesky_in_place();
	m.solve_lower_in_place(v.v);
	
	simd_t<T,S> u = v.v._vertical_dotn(v.v, v.x);
	v.x = u.sqrt();
    }

    inline void decholesky_in_place()
    {
	v.x = v.vertical_dot(v);
	m.multiply_lower_in_place(v.v);
	m.decholesky_in_place();
    }

    // Returns 1 if Cholesky factorization succeeded, 0 if poorly conditioned.
    // FIXME makes sense for T=float, but for T=double will we want to replace int by int64_t?
    inline simd_t<int,S> cholesky_in_place_checked(simd_t<T,S> epsilon)
    {
	simd_t<int,S> flags = m.cholesky_in_place_checked(epsilon);
	m.solve_lower_in_place(v.v);

	simd_t<T,S> u = v.v._vertical_dotn(v.v, v.x);
	simd_t<T,S> u0 = epsilon * v.x;

	v.x = u.max(u0).sqrt();
	flags = flags.bitwise_and(u.compare_gt(u0));

	return flags;
    }

    inline simd_ntuple<T,S,N> multiply_lower(const simd_ntuple<T,S,N> &t) const  { simd_ntuple<T,S,N> ret = t; multiply_lower_in_place(ret); return ret; }
    inline simd_ntuple<T,S,N> multiply_upper(const simd_ntuple<T,S,N> &t) const  { simd_ntuple<T,S,N> ret = t; multiply_upper_in_place(ret); return ret; }
    inline simd_ntuple<T,S,N> solve_lower(const simd_ntuple<T,S,N> &t) const     { simd_ntuple<T,S,N> ret = t; solve_lower_in_place(ret); return ret; }
    inline simd_ntuple<T,S,N> solve_upper(const simd_ntuple<T,S,N> &t) const     { simd_ntuple<T,S,N> ret = t; solve_upper_in_place(ret); return ret; }

    inline simd_trimatrix<T,S,N> cholesky() const    { simd_trimatrix<T,S,N> ret = *this; ret.cholesky_in_place(); return ret; }    
    inline simd_trimatrix<T,S,N> decholesky() const  { simd_trimatrix<T,S,N> ret = *this; ret.decholesky_in_place(); return ret; }    
};


template<typename T, unsigned int S>
struct simd_trimatrix<T,S,0>
{
    inline void setzero() { }
    inline void loadu(const T *p) { }
    inline void storeu(T *p) const { }
    inline void set1_slow(const T *p) { }

    inline simd_trimatrix<T,S,0> &operator+=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator-=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator*=(const simd_trimatrix<T,S,0> &t) { return *this; }
    inline simd_trimatrix<T,S,0> &operator/=(const simd_trimatrix<T,S,0> &t) { return *this; }

    inline simd_trimatrix<T,S,0> operator+(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator-(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator*(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }
    inline simd_trimatrix<T,S,0> operator/(const simd_trimatrix<T,S,0> &t) const { return simd_trimatrix<T,S,0>(); }

    inline simd_t<T,S> _vertical_sum(simd_t<T,S> x) const { return x; }

    inline simd_ntuple<T,S,0> multiply_lower_in_place(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> multiply_upper_in_place(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> multiply_symmetric(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> solve_lower_in_place(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }
    inline simd_ntuple<T,S,0> solve_upper_in_place(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0> (); }

    inline void horizontal_sum_in_place() { }
    inline void cholesky_in_place() { }
    inline void decholesky_in_place() { }

    inline simd_t<int,S> cholesky_in_place_checked(simd_t<T,S> epsilon) { return simd_t<int,S>(1); }
};


#endif // _SIMD_TRIMATRIX_HPP
