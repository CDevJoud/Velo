#pragma once

namespace velo {
	namespace core {
		template<typename T>
		class Intrusive;
	}

	template<typename T, typename F>
	auto safe_access(const core::Intrusive<T>& ptr, F&& fn) -> std::invoke_result_t<F, T&> {
		using R = std::invoke_result_t<F, T&>;

		if (ptr) {
			return std::invoke(std::forward<F>(fn), *ptr);
		}

		if constexpr (!std::is_void_v<R>) {
			return R{};
		}
	}

	template<typename T, typename F, typename R>
	R safe_access(const core::Intrusive<T>& ptr, F&& fn, R fallback) {
		if (ptr) {
			return std::invoke(std::forward<F>(fn), *ptr);
		}

		return fallback;
	}

	template<typename R, typename T, typename F>
	R safe_access_intr(const core::Intrusive<T>& ptr, F&& fn, R fallback = R{}) {
		if (ptr) {
			return fn(ptr);
		}
		return fallback;
	}

	template<typename T, typename F>
	inline void safe_access_intr(const core::Intrusive<T>& ptr, F&& fn) {
		if (ptr) {
			fn(ptr);
		}
	}
}
