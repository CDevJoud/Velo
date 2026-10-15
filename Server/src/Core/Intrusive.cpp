#include "Core/Intrusive.hpp"

namespace velo::core {

	IntrusiveCounted::IntrusiveCounted(const IntrusiveCounted&) : nIntrusiveCount(0) {}

	void IntrusiveCounted::incIntrusiveCount() const {
		++nIntrusiveCount;
	}

	void IntrusiveCounted::decIntrusiveCount() const {
		--nIntrusiveCount;
	}

	u32 IntrusiveCounted::getIntrusiveCount() const { return nIntrusiveCount.load(); }
}
