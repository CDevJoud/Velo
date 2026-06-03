#pragma once

namespace velo {
	template<typename R, typename T, typename F>
	R safe_access(const std::shared_ptr<T>& ptr, F&& fn, R fallback = R{}) {
		if (ptr) {
			return fn(ptr);
		}
		return fallback;
	}

	template<typename T, typename F>
	inline void safe_access(const std::shared_ptr<T>& ptr, F&& fn) {
		if (ptr) {
			fn(ptr);
		}
	}
}
