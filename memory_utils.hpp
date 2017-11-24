// Coming soon: an Allocator class (for std::vector, etc.) which uses aligned_alloc()

#include <memory>
#include <cstdlib>
#include <cstring>
#include <stdexcept>


// -------------------------------------------------------------------------------------------------
//
// aligned_alloc


template<typename T>
inline T *aligned_alloc(size_t nelts, size_t nalign=128, bool zero=true)
{
    if (nelts == 0)
        return NULL;

    void *p = NULL;
    if (posix_memalign(&p, nalign, nelts * sizeof(T)) != 0)
        throw std::runtime_error("couldn't allocate memory");

    if (zero)
	memset(p, 0, nelts * sizeof(T));

    return reinterpret_cast<T *> (p);
}


inline bool is_aligned(const void *ptr, uintptr_t nbytes)
{
    // According to C++11 spec, "uintptr_t" is an unsigned integer type
    // which is guaranteed large enough to represent a pointer.
    return (uintptr_t(ptr) % nbytes) == 0;
}


// -------------------------------------------------------------------------------------------------
//
// uptr<T> usage:
//
//   uptr<float> p = make_uptr<float> (nelts);


struct uptr_deleter {
    inline void operator()(const void *p) { free(const_cast<void *> (p)); }
};

template<typename T>
using uptr = std::unique_ptr<T[], uptr_deleter>;

template<typename T>
inline uptr<T> make_uptr(size_t nelts, size_t nalign=128, bool zero=true)
{
    T *p = aligned_alloc<T> (nelts, nalign, zero);
    return uptr<T> (p);
}


// -------------------------------------------------------------------------------------------------
//
// make_unique


// This is in C++14 but not C++11.
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
