#pragma once
#include "typedefs.hpp"
#include <vector>
#include <string>

namespace velo {
	template <typename T, size_t U>
	struct FixedArray {
		FixedArray() : size(U), typeSize(sizeof(T)){
			memset(arr, 0, size * typeSize);
		}
		T arr[U];
		const Qword size;
		const Byte typeSize;
	};
#pragma pack(push, 1)
	struct PreLoginPacket {
		~PreLoginPacket() {
			if (playerUID) {
				delete playerUID;
				playerUID = nullptr;
			}
			if (loginKey) {
				delete loginKey;
				loginKey = nullptr;
			}
		}
		Int16 lceNetworkVersion;
		wchar_t* loginKey;
		Byte friendsOnly;
		Byte playerCounts; // Client split screened players. max 4.
		Int32 playersVersion;
		Int64* playerUID; // array of player UID
		velo::Byte uniqueSaveName[14];
		Int32 serverSettings;
		Byte hostIndex;
		Int32 texturePackID;
	};
#pragma pack(pop)

	static constexpr Int32 PRE_LOGIN_PACKET_SIZE = sizeof(PreLoginPacket);

	class Packet {
	public:
		enum class ID : Uint8 {
			PreLogin = 2,

			Invalid = -1
		};
		Packet() = default;
		Packet(const Packet::ID& pID);
		Packet(const std::vector<Byte>& data);

		~Packet();

		Packet& writeInt32(const Int32& num);

		Packet& readInt8(Int8& num);
		Packet& readInt16(Int16& num);
		Packet& readInt32(Int32& num);

		Packet& readByte(Byte& num);
		Packet& readWord(Word& num);
		Packet& readDword(Dword& num);
		
		Packet& readWString(std::wstring& str);
		template <typename T, size_t U>
		Packet& readFixedArray(FixedArray<T, U>& arr);

		Packet& build();

		Packet::ID getID() const;

		std::vector<Byte>& getRawBytes();
		Int32& getReadIndex();
		void setReadIndex(Int32 index);

		Packet& parsePacket(void* packData);
	private:
		friend class TCPClient;
		//void updateID();
		Int32 nRIndex = 0;
		Packet::ID nPID = Packet::ID::Invalid;
		std::vector<Byte> bytes;
		bool bIsBuilt = false;
	};

	inline Packet& operator>>(Packet& p, std::wstring& str) {
		return p.readWString(str);
	}

	inline Packet& operator>>(Packet& p, Int8& num) {
		return p.readInt8(num);
	}
	inline Packet& operator>>(Packet& p, Int16& num) {
		return p.readInt16(num);
	}
	inline Packet& operator>>(Packet& p, Int32& num) {
		return p.readInt32(num);
	}

	inline Packet& operator>>(Packet& p, Byte& num) {
		return p.readByte(num);
	}
	template<typename T, size_t U>
	inline Packet& operator>>(Packet& p, FixedArray<T, U>& arr) {
		return p.readFixedArray(arr);
	}
	template<typename T, size_t U>
	inline Packet& Packet::readFixedArray(FixedArray<T, U>& arr) {
		for (int i = 0; i < arr.size; i++) {
			switch (arr.typeSize) {
			case 1:
				Packet::readByte(arr.arr[i]);
				break;
			};
		}
		return *this;
	}
}