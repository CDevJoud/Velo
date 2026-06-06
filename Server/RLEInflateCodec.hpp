#pragma once

#include "typedefs.hpp"

namespace velo {
	class RLEInflateCodec {
	public:
		RLEInflateCodec();
		~RLEInflateCodec();

		static bool compress(void* pDestination, Dword* dwDestSize, const void* pSource, Dword dwSrcSize);
		static bool compressRLE(void* pDestination, Dword* dwDestSize, const void* pSource, Dword dwSrcSize);
		static bool compressRLEAndInflate(void* pDestination, Dword * dwDestSize, const void* pSource, Dword dwSrcSize);


		static Qword getLastError();
	};
}

