#include <new>
#include <cstdlib>

#include "typedefs.hpp"
#include "Platform.hpp"

using velo::i32;
using velo::u32;
using velo::u64;
using velo::u8;
void* operator new[](size_t size, const char*, int, unsigned, const char*, int) {
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

void* operator new[](size_t size, size_t alignment, size_t, const char*, int, unsigned, const char*, int) {
#if defined(VELO_PLATFORM_WINDOWS)
    void* p = std::malloc(size);
#else
    size_t a = alignment < alignof(void*) ? alignof(void*) : alignment;
    void* p = std::aligned_alloc(a, (size + a - 1) / a * a);
#endif
    if (!p) throw std::bad_alloc();
    return p;
}

void operator delete[](void* p, const char*, int, unsigned, const char*, int) noexcept { std::free(p); }
void operator delete[](void* p, size_t, size_t, const char*, int, unsigned, const char*, int) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }