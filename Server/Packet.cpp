#include "Packet.hpp"

#include <bit>
namespace velo {
	velo::Packet::Packet(const Packet::ID& pID) :
		nPID(pID) {}
	Packet::Packet(const std::vector<Byte>& data) {
		bytes = data;
	}
	Packet::~Packet() {
		bytes.clear();
		bytes.shrink_to_fit();
	}
	Packet& Packet::writeInt32(const Int32& num) {
		this->bytes.insert(
			this->bytes.end(),
			reinterpret_cast<Byte*>(&const_cast<Int32&>(num)),
			reinterpret_cast<Byte*>(&const_cast<Int32&>(num) + sizeof(Int32))
		);
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
			Packet::setReadIndex(Packet::getReadIndex() + sizeof(Int16));
		}
		return *this;
	}
	Packet& Packet::readInt32(Int32& num) {
		if (!Packet::bytes.empty()) {
			num = *reinterpret_cast<const Int32*>(Packet::bytes.data() + Packet::getReadIndex());
			Packet::setReadIndex(Packet::getReadIndex() + sizeof(Int32));
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
	Packet& Packet::readWString(std::wstring& str) {
		Int16 len = 0;
		if (!Packet::bytes.empty()) {
			Packet::readInt16(len);
			len = std::byteswap(len);

			str.resize(len + 1);
			memcpy(str.data(), Packet::bytes.data() + Packet::getReadIndex(), len * sizeof(wchar_t));

			for (int i = 0; i < len; i++) {
				Int16 num = 0;
				Packet::readInt16(num);
				wchar_t character = num;
				character = std::byteswap(character);
				str[i] = character;
			}

		}
		return *this;
	}
	Packet& Packet::build() {
		if (Packet::bIsBuilt == false) {
			Int32 id = static_cast<Int32>(Packet::getID());
			Packet::bytes.insert(
				Packet::bytes.begin(),
				reinterpret_cast<char*>(&id),
				reinterpret_cast<char*>(id) + sizeof(Int32));
		}
		return *this;
	}
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
		case velo::Packet::ID::PreLogin:
		{
			PreLoginPacket* plp = (PreLoginPacket*)packData;
			if (plp != nullptr) {
				std::wstring loginKey;
				velo::FixedArray<velo::Byte, 14> uniqueSaveName;
				*this >> plp->lceNetworkVersion >> loginKey >> plp->friendsOnly >> plp->playersVersion >> plp->playerCounts >> uniqueSaveName >> plp->serverSettings >> plp->hostIndex >> plp->texturePackID;
				plp->lceNetworkVersion = std::byteswap(plp->lceNetworkVersion);
				memcpy(plp->uniqueSaveName, uniqueSaveName.arr, uniqueSaveName.size);
				plp->loginKey = new wchar_t[loginKey.length()] {};
				memcpy(plp->loginKey, loginKey.data(), loginKey.length() * sizeof(wchar_t));
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
}
