#pragma once

namespace velo {
	class NonCopyable {
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable operator==(const NonCopyable&) = delete;
	};
}
