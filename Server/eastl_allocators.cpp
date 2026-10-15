#include <new>

#include "typedefs.hpp"

using velo::i32;
using velo::u32;
using velo::u64;
using velo::u8;

void __cdecl operator delete[](void* p, const char* name, i32 flags, u32 dbgFlags, const char* file, i32 line) noexcept {
	delete[] static_cast<u8*>(p);
}

void __cdecl operator delete[](void* p, size_t size, u64 alignment, u64 offset, const char* name, i32 flags, u32 dbgFlags, const char* file, i32 line) noexcept {
	delete[] static_cast<u8*>(p);
}

void __cdecl operator delete[](void* p) noexcept {
	if (p != nullptr) {
		std::free(p);
		p = nullptr;
	}
}

void* __cdecl operator new[](size_t size, const char* name, int flags, u32 debugFlags, const char* file, i32 line) {
	return new u8[size];
}

void* __cdecl operator new[](size_t size, u64 alignment, u64 offset, char const* pName, int flags, u32 debugFlags, char const* file, i32 line) {
	return new u8[size];
}