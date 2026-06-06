#include "RLEInflateCodec.hpp"
#include "miniz.h"
#include <mutex>

namespace velo {
	static Uint8* s_n8CompressBuffer = nullptr;
	static constexpr Qword s_qwCompressBufferSize = 1024 * 100;
	static Qword s_qwRefCount = 0;
	static Qword s_qwLastError = 0;
	static std::mutex s_mutex;
	RLEInflateCodec::RLEInflateCodec() {
		if (s_n8CompressBuffer == nullptr) {
			s_n8CompressBuffer = new Uint8[s_qwCompressBufferSize]{};
			s_qwRefCount++;
		}
	}
	RLEInflateCodec::~RLEInflateCodec() {
		if (s_qwRefCount == 0) {
			delete s_n8CompressBuffer;
		}
		else {
			s_qwRefCount--;
		}
	}
	bool RLEInflateCodec::compress(void* pDestination, Dword* dwDestSize, const void* pSource, Dword dwSrcSize) {
		s_qwLastError = mz_compress(static_cast<Byte*>(pDestination), (mz_ulong*)dwDestSize, static_cast<const Byte*>(pSource), dwSrcSize);
		return (s_qwLastError == MZ_OK);
	}
	bool RLEInflateCodec::compressRLE(void* pDestination, Dword* dwDestSize, const void* pSource, Dword dwSrcSize) {
		std::lock_guard<std::mutex> lock(s_mutex);

		static const Dword s_max_compress_size = 1024 * 100;
		Dword dwRleBufSize = dwSrcSize * 2;
		Byte* dynRleBuf = nullptr;
		Byte* rleBuf = nullptr;

		if (dwRleBufSize <= s_max_compress_size) {
			rleBuf = s_n8CompressBuffer;
			dwRleBufSize = s_max_compress_size;
		}
		else {
			dynRleBuf = new Byte[dwRleBufSize];
			rleBuf = dynRleBuf;
		}

		const Byte* pucIn = static_cast<const Byte*>(pSource);
		const Byte* pucEnd = pucIn + dwSrcSize;
		Byte* pucOut = rleBuf;

		do {
			const Byte thisOne = *pucIn++;

			Dword count = 1;
			while ((pucIn != pucEnd) && (*pucIn == thisOne) && (count < 256)) {
				pucIn++;
				count++;
			}

			if (count <= 3) {
				if (thisOne == 255) {
					*pucOut++ = 255;
					*pucOut++ = count - 1;
				}
				else {
					for (auto i = 0u; i < count; i++) {
						*pucOut++ = thisOne;
					}
				}
			}
			else {
				*pucOut++ = 255;
				*pucOut++ = count - 1;
				*pucOut++ = thisOne;
			}
		} while (pucIn != pucEnd);
		const Dword rleSize = static_cast<Dword>(pucOut - rleBuf);

		if (rleSize <= *dwDestSize) {
			*dwDestSize = rleSize;
			memcpy(pDestination, rleBuf, *dwDestSize);
		}
		else {
			s_qwLastError = MZ_MEM_ERROR;
		}

		if (dynRleBuf != nullptr) {
			delete[] dynRleBuf;
		}

		s_qwLastError = MZ_OK;
		return (s_qwLastError == MZ_OK);
	}
	bool RLEInflateCodec::compressRLEAndInflate(void* pDestination, Dword* dwDestSize, const void* pSource, Dword dwSrcSize) {
		std::lock_guard<std::mutex> lock(s_mutex);

		static const Dword s_max_compress_size = 1024 * 100;
		Dword dwRleBufSize = dwSrcSize * 2;
		Byte* dynRleBuf = nullptr;
		Byte* rleBuf = nullptr;

		if (dwRleBufSize <= s_max_compress_size) {
			rleBuf = s_n8CompressBuffer;
			dwRleBufSize = s_max_compress_size;
		}
		else {
			dynRleBuf = new Byte[dwRleBufSize];
			rleBuf = dynRleBuf;
		}

		const Byte* pucIn = static_cast<const Byte*>(pSource);
		const Byte* pucEnd = pucIn + dwSrcSize;
		Byte* pucOut = rleBuf;

		do {
			const Byte thisOne = *pucIn++;

			Dword count = 1;
			while ((pucIn != pucEnd) && (*pucIn == thisOne) && (count < 256)) {
				pucIn++;
				count++;
			}

			if (count <= 3) {
				if (thisOne == 255) {
					*pucOut++ = 255;
					*pucOut++ = count - 1;
				}
				else {
					for (auto i = 0u; i < count; i++) {
						*pucOut++ = thisOne;
					}
				}
			}
			else {
				*pucOut++ = 255;
				*pucOut++ = count - 1;
				*pucOut++ = thisOne;
			}
		} while (pucIn != pucEnd);
		const Dword rleSize = static_cast<Dword>(pucOut - rleBuf);

		bool ret = RLEInflateCodec::compress(pDestination, dwDestSize, rleBuf, rleSize);

		if (dynRleBuf != nullptr) {
			delete[] dynRleBuf;
			dynRleBuf = nullptr;
		}

		return ret;
	}
	Qword RLEInflateCodec::getLastError() {
		std::lock_guard<std::mutex>lock (s_mutex);
		return s_qwLastError;
	}
}
