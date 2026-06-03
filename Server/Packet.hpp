#pragma once
#include "typedefs.hpp"
#include <vector>
#include <string>
#include "json.hpp"

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
	template <typename T>
	struct Array {
		Array() : arr(nullptr), size(0), typeSize(sizeof(T)) {

		}
		~Array() {
			if (arr != nullptr) {
				delete arr;
				arr = nullptr;
				size = 0;
			}
		}
		Qword size;
		T* arr;
		Byte typeSize;
	};
	struct KeepAlivePacket {
		Int32 id;
	};

	struct DebugOptionsPacket {
		Uint32 value;
	};

	struct DisconnectPacket {
		enum Reason {
			None = 0,
			Quitting,
			Closed,
			LoginTooLong,
			IllegalStance,
			IllegalPosition,
			MovedTooQuickly,
			NoFlying,
			Kicked,
			TimeOut,
			Overflow,
			EndOfStream,
			ServerFull,
			OutdatedServer,
			OutdatedClient,
			UnexpectedPacket,
			ConnectionCreationFailed,
			NoMultiplayerPrivilegesHost,
			NoMultiplayerPrivilegesJoin,
			NoUGC_AllLocal,
			NoUGC_Single_Local,
			ContentRestricted_AllLocal,
			ContentRestricted_Single_Local,
			NoUGC_Remote,
			NoFriendsInGame,
			Banned,
			NotFriendsWithHost,
			NATMismatch,
		};
		Reason reason;
	};

	struct ChunkVisibilityPacket {
		Int32 x, z;
		bool visibile;
	};

	struct BlockRegionUpdatePacket {
		Int32 x, y, z;
		Int32 xs, ys, zs;
		Array<Byte> buffer;
		Int32 levelIdx;
		bool bIsFullChunk; // 4J Added
	};

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
		char16_t* loginKey;
		Byte friendsOnly;
		Byte playerCounts; // Client split screened players. max 4.
		Int32 playersVersion;
		Int64* playerUID; // array of player UID
		velo::Byte uniqueSaveName[14];
		Int32 serverSettings;
		Byte hostIndex;
		Int32 texturePackID;
	};

	struct LoginPacket {
		~LoginPacket() {
			if (userName != nullptr) {
				delete userName;
				userName = nullptr;
			}
			if (pLevelType != nullptr) {
				delete pLevelType;
				pLevelType = nullptr;
			}
		}
		
		Int32 clientVersion;
		char16_t* userName;
		char16_t* pLevelType; // from the client LevelType class. Needs to be implemented later. set to default!
		Int64 seed;
		Int32 gameType;
		//bool isHardCore;
		Byte dimension;
		Byte mapHeight;
		Byte maxPlayers;
		Qword offlineXUID;
		Qword onlineXUID;
		bool friendsOnlyUGC;
		Dword ugcPlayerVersion;
		Byte difficulty;
		Int32 multiplayerInstanceID;
		Byte playerIndex;
		Dword playerSkinID;
		Dword playerCapeID;
		bool isGuest;
		bool newSeaLevel;
		Uint32 uiGamePrivileges;
		Int16 xzSize;
		Byte hellScale;
	};

	
#pragma pack(pop)

	static constexpr Int32 PRE_LOGIN_PACKET_SIZE = sizeof(PreLoginPacket);
	static constexpr Int32 KEEP_ALIVE_PACKET_SIZE = sizeof(KeepAlivePacket);
	static constexpr Int32 LOGIN_PACKET_SIZE = sizeof(LoginPacket);

	class Packet {
	public:
		enum class ID : Uint8 {
			KeepAlive = 0,
			Login,
			PreLogin,
			ChunkVisibility = 50,

			DebugOptions = 152,
			DisconnectPacket = 255,
			Invalid = 255
		};

		Packet() = default;
		Packet(const Packet::ID& pID);
		Packet(const std::vector<Byte>& data);

		~Packet();

		Packet& writeInt8(const Int8& num);
		Packet& writeInt16(const Int16& num);
		Packet& writeInt32(const Int32& num);
		Packet& writeInt64(const Int64& num);

		Packet& writeByte(const Byte& num);
		Packet& writeWord(const Word& num);
		Packet& writeDword(const Dword& num);
		Packet& writeQword(const Qword& num);

		Packet& writeWString(const std::u16string& str);
		template<typename T, size_t U>
		Packet& writeFixedArray(FixedArray<T, U>& arr);

		Packet& readInt8(Int8& num);
		Packet& readInt16(Int16& num);
		Packet& readInt32(Int32& num);
		Packet& readInt64(Int64& num);

		Packet& readByte(Byte& num);
		Packet& readWord(Word& num);
		Packet& readDword(Dword& num);
		Packet& readQword(Qword& num);

		Packet& readWString(std::u16string& str);
		template <typename T, size_t U>
		Packet& readFixedArray(FixedArray<T, U>& arr);

		//Packet& build();

		Packet::ID getID() const;

		std::vector<Byte>& getRawBytes();
		Int32& getReadIndex();
		void setReadIndex(Int32 index);

		Packet& parsePacket(void* packData);
		Packet& parsePacket(nlohmann::json& json);

		Packet& buildPacket(void* packData);
		Packet& buildPacket(const nlohmann::json& json);

		void reset();

		static const Packet createDisconnect(DisconnectPacket::Reason reason);
		static const Packet createDebugOptions(const Uint32 value = 0UL);
		static const Packet createKeepAlive(const Int32 id);
		static const Packet createChunkVisibility(const Int32 x, const Int32 z, bool visibile);
		static const Packet createPreLogin(Int16 lceNetworkVersion, const std::u16string& loginKey, Byte friendsOnly, Byte playerCounts, Int32 playersVersion, Int64* playersUID, FixedArray<Byte, 14>* uniqueSaveName, Int32 serverSettings, Byte hostIndex, Int32 texturePackID);
		static const Packet createLogin(
			Int32 clientVersion,
			const std::u16string& userName,
			const std::u16string& levelType, // set to default
			Int64 seed,
			Int32 gameType,
			//bool isHardCore,
			Byte dimension,
			Byte mapHeight,
			Byte maxPlayers,
			Qword offlineXUID,
			Qword onlineXUID,
			bool friendsOnlyUGC,
			Dword ugcPlayerVersion,
			Byte difficulty,
			Int32 multiplayerInstanceID,
			Byte playerIndex,
			Dword playerSkinID,
			Dword playerCapeID,
			bool isGuest,
			bool newSeaLevel,
			Uint32 uiGamePrivileges,
			Int32 xzSize,
			Int32 hellScale
		);
	private:
		friend class TCPClient;
		//void updateID();
		Int32 nRIndex = 0;
		Packet::ID nPID = Packet::ID::Invalid;
		std::vector<Byte> bytes;
		bool bIsBuilt = false;
	};

	inline Packet& operator>>(Packet& p, std::u16string& str) {
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
	inline Packet& operator>>(Packet& p, Int64& num) {
		return p.readInt64(num);
	}

	inline Packet& operator>>(Packet& p, Byte& num) {
		return p.readByte(num);
	}
	inline Packet& operator>>(Packet& p, Word& num) {
		return p.readWord(num);
	}
	inline Packet& operator>>(Packet& p, Dword& num) {
		return p.readDword(num);
	}
	inline Packet& operator>>(Packet& p, Qword& num) {
		return p.readQword(num);
	}
	template<typename T, size_t U>
	inline Packet& operator>>(Packet& p, FixedArray<T, U>& arr) {
		return p.readFixedArray(arr);
	}

	inline Packet& operator<<(Packet& p, const std::u16string& str) {
		return p.writeWString(str);
	}

	inline Packet& operator<<(Packet& p, const Int8& num) {
		return p.writeInt8(num);
	}
	inline Packet& operator<<(Packet& p, const Int16& num) {
		return p.writeInt16(num);
	}
	inline Packet& operator<<(Packet& p, const Int32& num) {
		return p.writeInt32(num);
	}
	inline Packet& operator<<(Packet& p, const Int64& num) {
		return p.writeInt64(num);
	}

	inline Packet& operator<<(Packet& p, const Byte& num) {
		return p.writeByte(num);
	}
	inline Packet& operator<<(Packet& p, const Word& num) {
		return p.writeWord(num);
	}
	inline Packet& operator<<(Packet& p, const Dword& num) {
		return p.writeDword(num);
	}
	inline Packet& operator<<(Packet& p, const Qword& num) {
		return p.writeQword(num);
	}
	template<typename T, size_t U>
	inline Packet& operator<<(Packet& p, FixedArray<T, U>& arr) {
		return p.writeFixedArray(arr);
	}

	template<typename T, size_t U>
	inline Packet& Packet::writeFixedArray(FixedArray<T, U>& arr) {
		for (int i = 0; i < arr.size; i++) {
			switch (arr.typeSize) {
			case 1:
				Packet::writeByte(arr.arr[i]);
				break;
			}
		}
		return *this;
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