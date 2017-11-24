#include <cstdlib>
#include <cstring>
#include <stdexcept>

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
