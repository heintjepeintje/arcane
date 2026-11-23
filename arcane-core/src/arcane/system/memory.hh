#ifndef AR_SYSTEM_MEMORY_HH_
#define AR_SYSTEM_MEMORY_HH_

#include <arcane/core/types.hh>
#include <memory>
#include <utility>

namespace arcane {

	class IAllocator {
	public:
		virtual ~IAllocator() = default;

		virtual void* allocate(u64 size, u64 alignment) = 0;
		virtual void deallocate(void* pointer) = 0;

		template<typename T>
		inline T* allocate_typed(u64 count) { return reinterpret_cast<T*>(allocate(count * sizeof(T), alignof(T))); }
	};

	class HeapAllocator : public IAllocator {
	public:
		void* allocate(u64 size, u64 alignment) override;
		void deallocate(void* pointer) override;
	};

	template<typename T>
	class SharedPtr;

	template<typename T>
	class UniquePtr {
	friend class SharedPtr<T>;
	public:
		template<typename... Args>
		static UniquePtr<T> create(IAllocator& allocator, Args&&... args);

		UniquePtr() = default;
		UniquePtr(IAllocator& allocator, T* pointer);
		UniquePtr(const UniquePtr<T>& copy) = delete;
		UniquePtr<T>& operator=(const UniquePtr<T>& copy) = delete;
		UniquePtr(UniquePtr<T>&& move);
		UniquePtr<T>& operator=(UniquePtr<T>&& move);
		~UniquePtr();

		void release();
		UniquePtr<T> move();
		inline bool is_valid() const { return _allocator != nullptr && _pointer != nullptr; }

		inline T* ptr() { return _pointer; }
		inline const T* ptr() const { return _pointer; }

		inline T& ref() { return *ptr(); }
		inline const T& ref() const { return *ptr(); }
	private:
		IAllocator* _allocator = nullptr;
		T* _pointer = nullptr;
	};

	template<typename T>
	class SharedPtr {
	public:
		template<typename... Args>
		static SharedPtr<T> create(IAllocator& allocator, Args&&... args);

		SharedPtr() = default;
		SharedPtr(UniquePtr<T>&& pointer);
		SharedPtr(const SharedPtr<T>& copy) = delete;
		SharedPtr<T>& operator=(const SharedPtr<T>& copy) = delete;
		SharedPtr(SharedPtr<T>&& copy);
		SharedPtr<T>& operator=(SharedPtr<T>&& copy);
		~SharedPtr();

		void release();
		SharedPtr<T> share() const;
		inline bool is_valid() const { return _allocator != nullptr && _data != nullptr && _data->pointer != nullptr; }

		inline T* ptr() const { return _data->pointer; }
		inline T& ref() const { return *ptr(); }

		inline uint64_t count() const { if (!is_valid()) return 0; else return _data->count; }
	private:
		struct Data {
			Data(T* pointer, u64 count) : pointer(pointer), count(count) { }
			T* pointer;
			u64 count;
		};

		IAllocator* _allocator = nullptr;	
		Data* _data = nullptr;
	};

	template<typename T>
	template<typename... Args>
	UniquePtr<T> UniquePtr<T>::create(IAllocator& allocator, Args&&... args) {
		UniquePtr<T> pointer;
		pointer._allocator = &allocator;
		pointer._pointer = pointer._allocator->allocate_typed<T>(1);
		std::construct_at(pointer._pointer, std::forward<Args>(args)...);

		return pointer;
	}

	template<typename T>
	UniquePtr<T>::UniquePtr(UniquePtr<T>&& move) : _allocator(move._allocator), _pointer(move._pointer) {
		move._allocator = nullptr;
		move._pointer = nullptr;
	}

	template<typename T>
	UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<T>&& move) {
		_allocator = move._allocator;
		_pointer = move._pointer;

		move._allocator = nullptr;
		move._pointer = nullptr;

		return *this;
	}

	template<typename T>
	UniquePtr<T>::~UniquePtr() {
		release();
	}

	template<typename T>
	void UniquePtr<T>::release() {
		if (!is_valid()) return;
		std::destroy_at(_pointer);
		_allocator->deallocate(_pointer);

		_allocator = nullptr;
		_pointer = nullptr;
	}

	template<typename T>
	template<typename... Args>
	SharedPtr<T> SharedPtr<T>::create(IAllocator& allocator, Args&&... args) {
		SharedPtr<T> pointer{};
		pointer._data = allocator.allocate_typed<Data>(1);
		pointer._data->pointer = allocator.allocate_typed<T>(1);
		pointer._data->count = 0;

		return pointer;
	}

	template<typename T>
	SharedPtr<T>::SharedPtr(UniquePtr<T>&& pointer) : _allocator(pointer._allocator) {
		_data = _allocator->template allocate_typed<Data>(1);
		_data->pointer = pointer._pointer;
		_data->count = 1;

		pointer._pointer = nullptr;		
	}

	template<typename T>
	SharedPtr<T>::SharedPtr(SharedPtr<T>&& move) : _allocator(move._allocator), _data(move._data) {
		move._allocator = nullptr;
		move._data = nullptr;
	}

	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& move) {
		release();

		_allocator = move._allocator;
		_data = move._data;

		move._allocator = nullptr;
		move._data = nullptr;

		return *this;
	}

	template<typename T>
	SharedPtr<T>::~SharedPtr() {
		release();
	}

	template<typename T>
	void SharedPtr<T>::release() {
		if (!is_valid()) return;

		_data->count -= 1;
		if (_data->count == 0) {
			_allocator->deallocate(_data->pointer);	
			_data->pointer = nullptr;

			_allocator->deallocate(_data);
		}

		_allocator = nullptr;
		_data = nullptr;
	}

	template<typename T>
	SharedPtr<T> SharedPtr<T>::share() const {
		if (!is_valid()) return SharedPtr<T>{};

		SharedPtr<T> result;
		result._allocator = _allocator;
		result._data = _data;

		_data->count += 1;

		return result;
	}

}

#endif // AR_SYSTEM_MEMORY_HH_
