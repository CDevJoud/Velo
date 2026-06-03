#include "Packet.hpp"
#include <iostream>
#include <bit>
namespace velo {

	bool HasKeys(
		const nlohmann::json& j,
		const std::vector<std::string>& keys) {
		for (const auto& key : keys) {
			if (!j.contains(key)) {
				return false;
			}
		}

		return true;
	}

	velo::Packet::Packet(const Packet::ID& pID) :
		nPID(pID) {}
	Packet::Packet(const std::vector<Byte>& data) {
		bytes = data;
	}
	Packet::~Packet() {
		bytes.clear();
		bytes.shrink_to_fit();
	}
	Packet& Packet::writeInt8(const Int8& num) {
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Int8&>(num)),
			reinterpret_cast<Byte*>(&const_cast<Int8&>(num)) + sizeof(Int8)
		);
		std::cout << "\tWrote Int8\n";
		return *this;
	}
	Packet& Packet::writeInt16(const Int16& num) {
		auto bigEndian = std::byteswap(num);
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Int16&>(bigEndian)),
			reinterpret_cast<Byte*>(&const_cast<Int16&>(bigEndian)) + sizeof(Int16)
		);
		std::cout << "\tWrote Int16\n";
		return *this;
	}
	Packet& Packet::writeInt32(const Int32& num) {
		auto bigEndian = std::byteswap(num);
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Int32&>(bigEndian)),
			reinterpret_cast<Byte*>(&const_cast<Int32&>(bigEndian)) + sizeof(Int32)
		);
		std::cout << "\tWrote Int32\n";
		return *this;
	}
	Packet& Packet::writeInt64(const Int64& num) {
		auto bigEndian = std::byteswap(num);
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Int64&>(bigEndian)),
			reinterpret_cast<Byte*>(&const_cast<Int64&>(bigEndian)) + sizeof(Int64)	
		);
		std::cout << "\tWrote Int64\n";
		return *this;
	}
	Packet& Packet::writeByte(const Byte& num) {
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Byte&>(num)),
			reinterpret_cast<Byte*>(&const_cast<Byte&>(num)) + sizeof(Byte)
		);
		std::cout << "\tWrote Byte\n";
		return *this;
	}
	Packet& Packet::writeWord(const Word& num) {
		auto bigEndian = std::byteswap(num);
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Word&>(bigEndian)),
			reinterpret_cast<Byte*>(&const_cast<Word&>(bigEndian)) + sizeof(Word)
		);
		std::cout << "\tWrote Word\n";
		return *this;
	}
	Packet& Packet::writeDword(const Dword& num) {
		auto bigEndian = std::byteswap(num);
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Dword&>(bigEndian)),
			reinterpret_cast<Byte*>(&const_cast<Dword&>(bigEndian)) + sizeof(Dword)
		);
		std::cout << "\tWrote Dword\n";
		return *this;
	}
	Packet& Packet::writeQword(const Qword& num) {
		auto bigEndian = std::byteswap(num);
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Qword&>(bigEndian)),
			reinterpret_cast<Byte*>(&const_cast<Qword&>(bigEndian)) + sizeof(Qword)
		);
		std::cout << "\tWrote Qword\n";
		return *this;
	}
	Packet& Packet::writeWString(const std::u16string& str) {
		Packet::writeInt16(static_cast<Int16>(str.length()));
		for (char16_t ch : str) {
			char16_t swapped = std::byteswap(ch);
			const Byte* data = reinterpret_cast<const Byte*>(&swapped);

			Packet::bytes.insert(
				Packet::bytes.end(),
				data,
				data + sizeof(char16_t));
		}
		std::cout << "\tWrote WString\n";
		return *this;
	}
	Packet& Packet::readInt8(Int8& num) {
		if (!Packet::bytes.empty()) {
			num = *reinterpret_cast<const Int8*>(Packet::bytes.data() + Packet::getReadIndex());
			Packet::setReadIndex(Packet::getReadIndex() + sizeof(Int8));
		}
		return *this;
	}
	Packet& Packet::readInt16(Int16& num) {
		if (!Packet::bytes.empty()) {
			num = *reinterpret_cast<const Int16*>(Packet::bytes.data() + Packet::getReadIndex());
			num = std::byteswap(num); // little endian
			Packet::setReadIndex(Packet::getReadIndex() + sizeof(Int16));
		}
		return *this;
	}
	Packet& Packet::readInt32(Int32& num) {
		if (!Packet::bytes.empty()) {
			num = *reinterpret_cast<const Int32*>(Packet::bytes.data() + Packet::getReadIndex());
			num = std::byteswap(num); // little endian
			Packet::setReadIndex(Packet::getReadIndex() + sizeof(Int32));
		}
		return *this;
	}
	Packet& Packet::readInt64(Int64& num) {
		if (!Packet::bytes.empty()) {
			num = *reinterpret_cast<const Int64*>(Packet::bytes.data() + Packet::getReadIndex());
			num = std::byteswap(num); // little endian
			Packet::setReadIndex(Packet::getReadIndex() + sizeof(Int64));
		}
		return *this;
	}
	Packet& Packet::readByte(Byte& num) {
		Int8 _num = num; Packet::readInt8(_num); num = _num; return *this;
	}
	Packet& Packet::readWord(Word& num) {
		Int16 _num = num; Packet::readInt16(_num); num = _num; return *this;
	}
	Packet& Packet::readDword(Dword& num) {
		Int32 _num = num; Packet::readInt32(_num); num = _num; return *this;
	}
	Packet& Packet::readQword(Qword& num) {
		Int64 _num = num; Packet::readInt64(_num); num = _num; return *this;
	}
	Packet& Packet::readWString(std::u16string& str) {
		Int16 len = 0;
		if (!Packet::bytes.empty()) {
			Packet::readInt16(len);

			str.resize(len + 1);
			memcpy(str.data(), Packet::bytes.data() + Packet::getReadIndex(), len * sizeof(char16_t));

			for (int i = 0; i < len; i++) {
				Int16 character = 0;
				Packet::readInt16(character);
				str[i] = character;
			}

		}
		return *this;
	}
	/*Packet& Packet::build() {
		if (Packet::bIsBuilt == false) {
			Int32 id = static_cast<Int32>(Packet::getID());
			Packet::bytes.insert(
				Packet::bytes.begin(),
				reinterpret_cast<char*>(&id),
				reinterpret_cast<char*>(id) + sizeof(Int32));
		}
		return *this;
	}*/
	Packet::ID Packet::getID() const {
		return this->nPID;
	}
	
	std::vector<Byte>& Packet::getRawBytes() {
		return this->bytes;
	}
	Int32& Packet::getReadIndex() {
		return this->nRIndex;
	}
	void Packet::setReadIndex(Int32 index) {
		this->nRIndex = index;
	}
	Packet& Packet::parsePacket(void* packData) {
		switch (nPID) {
		case velo::Packet::ID::DebugOptions:
		{
			DebugOptionsPacket* dop = (DebugOptionsPacket*)packData;
			if (dop != nullptr) {
				*this >> dop->value;
				return *this;
			}
		}
		break;
		case velo::Packet::ID::PreLogin:
		{
			PreLoginPacket* plp = (PreLoginPacket*)packData;
			if (plp != nullptr) {
				std::u16string loginKey;
				velo::FixedArray<velo::Byte, 14> uniqueSaveName;
				*this 
					>> plp->lceNetworkVersion 
					>> loginKey 
					>> plp->friendsOnly 
					>> plp->playersVersion 
					>> plp->playerCounts 
					>> uniqueSaveName 
					>> plp->serverSettings 
					>> plp->hostIndex 
					>> plp->texturePackID;

				memcpy(plp->uniqueSaveName, uniqueSaveName.arr, uniqueSaveName.size * uniqueSaveName.typeSize);
				plp->loginKey = new char16_t[loginKey.length() + 1] {};
				memcpy(plp->loginKey, loginKey.data(), loginKey.length() * sizeof(char16_t));
				
				return *this;
			}
		}
			break;
		case velo::Packet::ID::Login:
		{
			LoginPacket* lp = (LoginPacket*)packData;
			if (lp != nullptr) {
				std::u16string userName;
				std::u16string pLevelType;
				Byte bFriendsOnlyUGC = 0;
				Byte bIsGuest = 0;
				Byte bNewSeaLevel = 0;
				*this >> lp->clientVersion				;
				*this >> userName							;
				*this >> pLevelType						;
				*this >> lp->seed							;
				*this >> lp->gameType						;
				*this >> lp->dimension					;
				*this >> lp->mapHeight					;
				*this >> lp->maxPlayers					;
				*this >> lp->offlineXUID					;
				*this >> lp->onlineXUID					;
				*this >> bFriendsOnlyUGC					;
				*this >> lp->ugcPlayerVersion				;
				*this >> lp->difficulty					;
				*this >> lp->multiplayerInstanceID		;
				*this >> lp->playerIndex					;
				*this >> lp->playerSkinID					;
				*this >> lp->playerCapeID					;
				*this >> bIsGuest							;
				*this >> bNewSeaLevel						;
				*this >> lp->uiGamePrivileges				;
				*this >> lp->xzSize						;
				*this >> lp->hellScale;
				lp->userName = new char16_t[userName.length() + 1] {};
				memcpy(lp->userName, userName.data(), userName.length() * sizeof(char16_t));
				lp->pLevelType = new char16_t[pLevelType.length() + 1] {};
				memcpy(lp->pLevelType, pLevelType.data(), pLevelType.length() * sizeof(char16_t));
				lp->friendsOnlyUGC = (bool)bFriendsOnlyUGC;
				lp->isGuest = (bool)bIsGuest;
				lp->newSeaLevel = (bool)bNewSeaLevel;
				
				return *this;
			}
		}
			break;
		case velo::Packet::ID::Invalid:
			break;
		default:
			break;
		}
	}
	Packet& Packet::parsePacket(nlohmann::json& json) {
		switch (nPID) {
		case velo::Packet::ID::PreLogin:
		{
			PreLoginPacket plp{};
			Packet::parsePacket(&plp);
			json["lceNetworkVersion"] = plp.lceNetworkVersion;
			json["loginKey"] = plp.loginKey;
			json["friendsOnly"] = plp.friendsOnly;
			json["playerVersion"] = plp.playersVersion;
			json["playerCounts"] = plp.playerCounts;
			json["uniqueSaveName"] = (size_t)plp.uniqueSaveName; // we would like to get the ptr not inner content
			json["serverSettings"] = plp.serverSettings;
			json["hostIndex"] = plp.hostIndex;
			json["texturePackID"] = plp.texturePackID;
		}
		break;
		case velo::Packet::ID::Invalid:
			break;
		default:
			break;
		}
		return *this;
	}
	Packet& Packet::buildPacket(void* packData) {
		switch (nPID) {
		case velo::Packet::ID::DisconnectPacket:
		{
			DisconnectPacket* dp = (DisconnectPacket*)packData;
			if (dp != nullptr) {
				*this << static_cast<Byte>(velo::Packet::ID::DebugOptions)
					<< dp->reason;
			}
		}
		break;
		case velo::Packet::ID::DebugOptions:
		{
			DebugOptionsPacket* dop = (DebugOptionsPacket*)packData;
			if (dop != nullptr) {
				*this << static_cast<Byte>(velo::Packet::ID::DebugOptions)
					<< dop->value;
			}
		}
		break;
		case velo::Packet::ID::KeepAlive:
		{
			KeepAlivePacket* kap = (KeepAlivePacket*)packData;
			if (kap != nullptr) {
				*this << static_cast<Byte>(velo::Packet::ID::KeepAlive)
					  << kap->id;
			}
		}
		break;
		case velo::Packet::ID::ChunkVisibility:
		{
			ChunkVisibilityPacket* cvp = (ChunkVisibilityPacket*)packData;
			if (cvp != nullptr) {
				*this << static_cast<Byte>(velo::Packet::ID::ChunkVisibility)
					<< cvp->x
					<< cvp->z
					<< Byte(cvp->visibile);
			}
		}
		break;
		case velo::Packet::ID::PreLogin:
		{
			PreLoginPacket* plp = (PreLoginPacket*)packData;
			if (plp != nullptr) {
				velo::FixedArray<velo::Byte, 14> uniqueSaveName;
				memcpy(uniqueSaveName.arr, plp->uniqueSaveName, 14);
				*this 
					<< static_cast<Byte>(velo::Packet::ID::PreLogin) 
					<< plp->lceNetworkVersion 
					<< plp->loginKey 
					<< plp->friendsOnly 
					<< plp->playersVersion 
					<< plp->playerCounts 
					<< uniqueSaveName 
					<< plp->serverSettings 
					<< plp->hostIndex 
					<< plp->texturePackID;
			}
		}
		break;

		case velo::Packet::ID::Login:
		{
			LoginPacket* lp = (LoginPacket*)packData;
			if (lp != nullptr) {
				*this
					<< static_cast<Byte>(velo::Packet::ID::Login)
					<< lp->clientVersion
					<< lp->userName
					<< lp->pLevelType // this should always be nullptr from the dedicated server software
					<< lp->seed
					<< lp->gameType
					//<< lp->isHardCore
					<< lp->dimension
					<< lp->mapHeight
					<< lp->maxPlayers
					<< lp->offlineXUID
					<< lp->onlineXUID
					<< (Byte)lp->friendsOnlyUGC
					<< lp->ugcPlayerVersion
					<< lp->difficulty
					<< lp->multiplayerInstanceID
					<< lp->playerIndex
					<< lp->playerSkinID
					<< lp->playerCapeID
					<< (Byte)lp->isGuest
					<< (Byte)lp->newSeaLevel
					<< lp->uiGamePrivileges
					<< lp->xzSize
					<< lp->hellScale;
			}
		}
		break;

		/*case velo::Packet::ID::Invalid:
			break;*/
		default:
			break;
		}
		Int32 packsize = bytes.size();
		Int32 swapbytes = std::byteswap(packsize);
		bytes.insert(
			bytes.begin(),
			reinterpret_cast<const Byte*>(&swapbytes),
			reinterpret_cast<const Byte*>(&swapbytes) + sizeof(Int32)
		);
		return *this;
	}
	Packet& Packet::buildPacket(const nlohmann::json& json) {
		switch (nPID) {
		case velo::Packet::ID::PreLogin:
		{
			if (HasKeys(json, { "lceNetworkVersion", "loginKey", "friendsOnly", "playersVersion", "playerCounts", "uniqueSaveName", "serverSettings", "hostIndex", "texturePackID"})) {
				PreLoginPacket plp{};
				std::string loginKey;
				plp.lceNetworkVersion = json["lceNetworkVersion"];

				loginKey = json["loginKey"];
				std::u16string wLoginKey(loginKey.begin(), loginKey.end()); // we use std::u16string as wchar_t on linux is 4 bytes
				plp.loginKey = new char16_t[wLoginKey.length()] {};
				memcpy(plp.loginKey, wLoginKey.data(), wLoginKey.length() + sizeof(char16_t));

				plp.friendsOnly = json["friendsOnly"];
				plp.playersVersion = json["playersVersion"];
				plp.playerCounts = json["playerCounts"];

				void* ptr = (void*)json["uniqueSaveName"].get<size_t>(); // get the ptr
				memcpy(plp.uniqueSaveName, ptr, 14);

				plp.serverSettings = json["serverSettings"];
				plp.hostIndex = json["hostIndex"];
				plp.texturePackID = json["texturePackID"];

				Packet::buildPacket(&plp);
			}
			else {
				//Error
			}
		}
		break;
		case velo::Packet::ID::Invalid:
			break;
		default:
			break;
		}
		return *this;
	}
	void Packet::reset() {
		nRIndex = 0;
		nPID = Packet::ID::Invalid;
		bytes.clear();
		bytes.shrink_to_fit();
		bIsBuilt = false;
	}
	const Packet Packet::createDisconnect(DisconnectPacket::Reason reason) {
		Packet p = Packet::ID::DisconnectPacket;
		DisconnectPacket dp{};
		dp.reason = reason;
		p.buildPacket(&dp);
		return p;
	}
	const Packet Packet::createDebugOptions(const Uint32 value) {
		Packet p = Packet::ID::DebugOptions;
		DebugOptionsPacket dop{};
		dop.value = value;
		p.buildPacket(&dop);
		return p;
    }
    const Packet Packet::createKeepAlive(const Int32 id) {
		Packet p = Packet::ID::KeepAlive;
		KeepAlivePacket kap{};
		kap.id = id;
		p.buildPacket(&kap);
		return p;
	}
	const Packet Packet::createChunkVisibility(const Int32 x, const Int32 z, bool visibile) {
		Packet p = Packet::ID::ChunkVisibility;
		ChunkVisibilityPacket cvp{};
		cvp.x = x;
		cvp.z = z;
		cvp.visibile = visibile;
		p.buildPacket(&cvp);
		return p;
	}
	const Packet Packet::createPreLogin(
		Int16 lceNetworkVersion, 
		const std::u16string& loginKey, 
		Byte friendsOnly, 
		Byte playerCounts, 
		Int32 playersVersion, 
		Int64* playersUID, 
		FixedArray<Byte, 14>* uniqueSaveName, 
		Int32 serverSettings, 
		Byte hostIndex, 
		Int32 texturePackID) {
		FixedArray<Byte, 14>* _uniqueSaveName = nullptr;
		bool passedNullptr = false;
		if (uniqueSaveName == nullptr) {
			_uniqueSaveName = new FixedArray<Byte, 14>();
			passedNullptr = true;
		}
		else {
			_uniqueSaveName = uniqueSaveName;
		}
		Packet p(Packet::ID::PreLogin);
		PreLoginPacket plp{};
		plp.lceNetworkVersion = lceNetworkVersion;
		plp.loginKey = new char16_t[loginKey.length() + 1] {};
		memcpy(plp.loginKey, loginKey.data(), loginKey.length() * sizeof(char16_t));
		plp.friendsOnly = friendsOnly;
		plp.playerCounts = playerCounts;
		plp.playerUID = playersUID;
		memcpy(plp.uniqueSaveName, _uniqueSaveName->arr, _uniqueSaveName->size * _uniqueSaveName->typeSize);
		plp.serverSettings = serverSettings;
		plp.hostIndex = hostIndex;
		plp.texturePackID = texturePackID;
		p.buildPacket(&plp);
		if (passedNullptr)
			delete _uniqueSaveName;
		return p;
	}
	const Packet Packet::createLogin(
		Int32 clientVersion, 
		const std::u16string& userName, 
		const std::u16string& levelType, 
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
	) {
		Packet p(Packet::ID::Login);
		LoginPacket lp{};
		lp.clientVersion = clientVersion;
		lp.userName = new char16_t[userName.length() + 1] {};
		memcpy(lp.userName, userName.data(), userName.length() * sizeof(char16_t));
		lp.pLevelType = new char16_t[levelType.length() + 1] {};
		memcpy(lp.pLevelType, levelType.data(), levelType.length() * sizeof(char16_t));
		lp.seed = seed;
		lp.gameType = gameType;
		//lp.isHardCore = isHardCore;
		lp.dimension = dimension;
		lp.mapHeight = mapHeight;
		lp.maxPlayers = maxPlayers;
		lp.offlineXUID = offlineXUID;
		lp.onlineXUID = onlineXUID;
		lp.friendsOnlyUGC = friendsOnlyUGC;
		lp.ugcPlayerVersion = ugcPlayerVersion;
		lp.difficulty = difficulty;
		lp.multiplayerInstanceID = multiplayerInstanceID;
		lp.playerIndex = playerIndex;
		lp.playerSkinID = playerSkinID;
		lp.playerCapeID = playerCapeID;
		lp.isGuest = isGuest;
		lp.newSeaLevel = newSeaLevel;
		lp.uiGamePrivileges = uiGamePrivileges;
		lp.xzSize = xzSize;
		lp.hellScale = hellScale;
		p.buildPacket(&lp);
		return p;
	}
}
