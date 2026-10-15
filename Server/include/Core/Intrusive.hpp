#pragma once

#include <utility>
#include <atomic>
#include <type_traits>

#include "typedefs.hpp"

namespace velo::core {
	class IntrusiveCounted {
	public:
		IntrusiveCounted() = default;
		virtual ~IntrusiveCounted() = default;

		IntrusiveCounted(const IntrusiveCounted&);

		IntrusiveCounted& operator=(const IntrusiveCounted&) {
			nIntrusiveCount.store(0);
			return *this;
		}

		void incIntrusiveCount() const;

		void decIntrusiveCount() const;

		u32 getIntrusiveCount() const;

	private:
		mutable std::atomic<u32> nIntrusiveCount = 0;
	};

	template<typename T>
	class Intrusive {
	public:
		Intrusive() = default;
		~Intrusive() {
			Intrusive::release();
		}

		Intrusive(std::nullptr_t n) : instance(nullptr) {}

		Intrusive(const Intrusive<T>& other) : instance(other.instance) { Intrusive::retain(); }

		Intrusive<T>& operator=(const Intrusive<T>& other) {
			if (this != &other) {
				Intrusive::release();
				Intrusive::instance = other.instance;
				Intrusive::retain();
			}
			return *this;
		}

		Intrusive(Intrusive<T>&& other) noexcept : instance(other.instance) { other.instance = nullptr; }

		Intrusive(T* instance) :instance(instance) {
			static_assert(std::is_base_of<IntrusiveCounted, T>::value, "Class is not IntrusiveCounted!");

			Intrusive::retain();
		}

		operator bool() { return Intrusive::instance != nullptr; }
		operator bool() const { return Intrusive::instance != nullptr; }

		T* operator->() { return Intrusive::instance; }
		T* operator->() const { return Intrusive::instance; }

		T& operator*() { return *Intrusive::instance; }
		const T& operator*() const { return *Intrusive::instance; };

		T* getRawPtr() { return Intrusive::instance; }
		const T* getRawPtr() const { return Intrusive::instance; }

		void reset(T* instance = nullptr) {
			Intrusive::release();
			Intrusive::instance = instance;
		}

		template<typename T2> requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>) Intrusive<T2> as() const { return Intrusive<T2>(*this); }
		template<typename... Args>
		static Intrusive<T> make(Args&&... args) { return Intrusive<T>(new (std::nothrow) T(std::forward<Args>(args)...)); }

		bool operator==(const Intrusive<T>& other) const { return instance == other.instance; }

		bool operator!=(const Intrusive<T>& other) const { return !(*this == other); }

		bool equalObject(const Intrusive<T>& other) {
			if (!Intrusive::instance || !other.instance) {
				return false;
			}
		}

		void retain() const {
			if (Intrusive::instance) {
				Intrusive::instance->incIntrusiveCount();
			}
		}

		void release() const {
			if (Intrusive::instance) {

				Intrusive::instance->decIntrusiveCount();

				if (Intrusive::instance->getIntrusiveCount() == 0) {
					delete Intrusive::instance;
					Intrusive::instance = nullptr;
				}
			}
		}

		template<class T2>
		friend class Intrusive;

		mutable T* instance = nullptr;
	};

	template<typename U, typename T>
	Intrusive<U> dynamicPtrCast(const Intrusive<T>& ptr) {
		const U* casted = dynamic_cast<const U*>(ptr.getRawPtr());
		return (!casted) ? Intrusive<U>(nullptr) : Intrusive<U>(const_cast<U*>(casted));
	}
}
