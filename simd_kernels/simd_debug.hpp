#include <iostream>
#include "simd_t.hpp"


template<class T, unsigned int S, unsigned int M>
struct _simd_t_writer 
{
    static inline void write(std::ostream &os, simd_t<T,S> x)
    {
	_simd_t_writer<T,S,M-1>::write(os, x);
	os << ", " << x.template extract<M-1>();
    }
};


template<class T, unsigned int S>
struct _simd_t_writer<T,S,1>
{
    static inline void write(std::ostream &os, simd_t<T,S> x) 
    { 
	os << x.template extract<0> ();
    }
};


template<typename T, unsigned int S>
std::ostream &operator<<(std::ostream &os, simd_t<T,S> x)
{
    os << "[";
    _simd_t_writer<T,S,S>::write(os, x);
    os << "]";
    return os;
}
