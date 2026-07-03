#pragma once
#include <utility>
#include <atomic>
#include <type_traits>

namespace velo {
	class IntrusiveCounted {
	public:
		IntrusiveCounted() = default;
		virtual ~IntrusiveCounted() = default;

		IntrusiveCounted(const IntrusiveCounted&) : IntrusiveCount(0) {

		}

		IntrusiveCounted& operator=(const IntrusiveCounted&) {
			IntrusiveCount.store(0);
			return *this;
		}

		void incIntrusiveCount() const {
			++IntrusiveCount;
		}
		void decIntrusiveCount() const {
			--IntrusiveCount;
		}

		uint32_t getIntrusiveCount() const { return IntrusiveCount.load(); }

	private:
		mutable std::atomic<uint32_t> IntrusiveCount = 0;
	};

	template <typename T>
	class Intrusive {
	public:
		Intrusive() = default;
		~Intrusive() {
			release();
		}


		Intrusive(std::nullptr_t n) :
			instance(nullptr) {

		}

		Intrusive(const Intrusive<T>& other) : instance(other.instance){
			retain();
		}

		Intrusive<T>& operator=(const Intrusive<T>& other) {
			if (this != &other) {
				release();
				instance = other.instance;
				retain();
			}
			return *this;
		}

		Intrusive(Intrusive<T>&& other) noexcept
			: instance(other.instance) {
			other.instance = nullptr;
		}

		Intrusive(T* instance)
			: instance(instance) {
			static_assert(std::is_base_of<IntrusiveCounted, T>::value, "Class is not IntrusiveCounted!");

			retain();
		}

		template<typename T2>
		Intrusive& operator=(Intrusive<T2>&& other) {
			release();

			instance = other.instance;
			other.instance = nullptr;
			return *this;
		}

		operator bool() { return instance != nullptr; }
		operator bool() const { return instance != nullptr; }

		T* operator->() { return instance; }
		T* operator->() const { return instance; }

		T& operator*() { return *instance; }
		const T& operator*() const { return *instance; }

		T* getRawPtr() { return instance; }
		const T* getRawPtr() const { return instance; }

		void reset(T* instance = nullptr) {
			release();
			this->instance = instance;
		}

		template<typename T2>
			requires(std::is_base_of_v<T2, T> || std::is_base_of_v<T, T2>)
		Intrusive<T2> as() const {
			return Intrusive<T2>(*this);
		}

		template<typename... Args>
		static Intrusive<T> make(Args&&...args) {
			return Intrusive<T>(new (std::nothrow) T(std::forward<Args>(args)...));
		}

		bool operator==(const Intrusive<T>& other) const {
			return instance == other.instance;
		}

		bool operator!=(const Intrusive<T>& other) const {
			return !(*this == other);
		}

		bool equalObject(const Intrusive<T>& other) {
			if (!instance || !other.instance) {
				return false;
			}

			return instance == other.instance;
		}

		void retain() const {
			if (instance) {
				instance->incIntrusiveCount();
			}
		}

		void release() const {
			if (instance) {
				instance->decIntrusiveCount();

				if (instance->getIntrusiveCount() == 0) {
					delete instance;
					instance = nullptr;
				}
			}
		}

		template<class T2>
		friend class Intrusive;

		mutable T* instance = nullptr;
	};

	template <typename U, typename T>
	Intrusive<U> dynamicPtrCast(const Intrusive<T>& ptr) {
		const U* casted = dynamic_cast<const U*>(ptr.getRawPtr());
		
		if (!casted)
			return Intrusive<U>(nullptr);

		return Intrusive<U>(const_cast<U*>(casted));
	}
}