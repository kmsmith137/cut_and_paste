#include "simd_t.hpp"

#ifndef _SIMD_NTUPLE_HPP
#define _SIMD_NTUPLE_HPP


template<typename T, unsigned int S, unsigned int N>
struct simd_ntuple
{
    simd_ntuple<T,S,N-1> v;
    simd_t<T,S> x;

    simd_ntuple() { }

    inline void setzero()
    {
	v.setzero();
	x.setzero();
    }

    inline void loadu(const T *p)
    {
	v.loadu(p);
	x.loadu(p+(N-1)*S);
    }

    inline void storeu(T *p) const
    {
	v.storeu(p);
	x.storeu(p+(N-1)*S);
    }

    inline simd_ntuple<T,S,N> &operator+=(const simd_ntuple<T,S,N> &t)
    {
	v += t.v;
	x += t.x;
	return *this;
    }

    inline simd_ntuple<T,S,N> &operator-=(const simd_ntuple<T,S,N> &t)
    {
	v -= t.v;
	x -= t.x;
	return *this;
    }

    inline simd_ntuple<T,S,N> &operator*=(const simd_ntuple<T,S,N> &t)
    {
	v *= t.v;
	x *= t.x;
	return *this;
    }

    inline simd_ntuple<T,S,N> &operator/=(const simd_ntuple<T,S,N> &t)
    {
	v /= t.v;
	x /= t.x;
	return *this;
    }

    inline simd_ntuple<T,S,N> operator+(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = v + t.v;
	ret.x = x + t.x;
	return ret;
    }

    inline simd_ntuple<T,S,N> operator-(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = v - t.v;
	ret.x = x - t.x;
	return ret;
    }

    inline simd_ntuple<T,S,N> operator*(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = v * t.v;
	ret.x = x * t.x;
	return ret;
    }

    inline simd_ntuple<T,S,N> operator/(const simd_ntuple<T,S,N> &t) const
    {
	simd_ntuple<T,S,N> ret;
	ret.v = v / t.v;
	ret.x = x / t.x;
	return ret;
    }

    inline simd_t<T,S> vertical_sum() const  { return v.vertical_sum() + x; }

    inline T sum() const { return this->vertical_sum().sum(); }
    
    inline T compare(const T *p) const
    {
	const simd_ntuple<T,S,N> &a = *this;
	
	simd_ntuple<T,S,N> b;
	b.loadu(p);
	
	simd_ntuple<T,S,N> t = (a-b) * (a-b);
	float num = t.sum();
	
	t = a*a + b*b;
	float den = t.sum();
	
	return (den > 0.0) ? sqrt(num/den) : 0.0;
    }
};


template<typename T, unsigned int S> 
struct simd_ntuple<T,S,0> 
{ 
    inline void setzero() { }
    inline void loadu(const T *p) { }
    inline void storeu(T *p) const { }

    inline simd_ntuple<T,S,0> operator+(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0>(); }
    inline simd_ntuple<T,S,0> operator-(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0>(); }
    inline simd_ntuple<T,S,0> operator*(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0>(); }
    inline simd_ntuple<T,S,0> operator/(const simd_ntuple<T,S,0> &t) const { return simd_ntuple<T,S,0>(); }

    inline simd_t<T,S> vertical_sum() const { return 0; }
};


//template<typename T, unsigned int S> 
//inline simd_t<T,S> simd_ntuple<T,S,1>::vertical_sum() const { return this->x; }


#endif // _NTUPLE_HPP
