#ifndef AR_DATA_BUFFER_HH_
#define AR_DATA_BUFFER_HH_

#include <arcane/core/types.hh>
#include "./view.hh"
#include "./slice.hh"

#include <cstring>

namespace arcane {

	template<typename T>
	void copy(const View<T>& source, const Slice<T>& destination);

	template<typename T>
	void copy_bytes(const View<T>& source, const Slice<T>& destination);

	template<typename T>
	void copy(const View<T>& source, const Slice<T>& destination) {
		AR_TRACE();
		const u64 copy_length = source.length() > destination.length() ? destination.length() : source.length();
		for (u64 i = 0; i < copy_length; i++) {
			destination.at(i) = source.at(i);
		}
	}

	template<typename T>
	void copy_bytes(const View<T>& source, const Slice<T>& destination) {
		AR_TRACE();
		const u64 copy_length = source.length() > destination.length() ? destination.length() : source.length();
		std::memcpy(destination.data(), source.data(), copy_length);	
	}

	template<typename T>
	class Buffer {
	public:
		static Buffer allocate(u64 length);

		Buffer();
		Buffer(const Buffer<T>& copy) = delete;
		Buffer<T>& operator=(const Buffer<T>& copy) = delete;
		Buffer(Buffer<T>&& move);
		Buffer<T>& operator=(Buffer<T>&& move);
		~Buffer();

		void deallocate();

		View<T> view(u64 offset, u64 length) const;
		Slice<T> slice(u64 offset, u64 length);

		inline View<T> view() const { return view(0, _length); }
		inline Slice<T> slice() { return slice(0, _length); }

		inline T* data() { return _data; }
		inline const T* data() const { return _data; }
		inline u64 length() const { return _length; }
	private:
		T* _data;
		u64 _length;
	};

	template<typename T>
	Buffer<T> Buffer<T>::allocate(u64 length) {
		Buffer<T> buffer{};
		buffer._data = new T[length];
		buffer._length = length;
		
		return buffer;
	}

	template<typename T>
	Buffer<T>::Buffer() : _data(nullptr), _length(0) { }

	template<typename T>
	Buffer<T>::Buffer(Buffer<T>&& move) : _data(move._data), _length(move._length) {
		move._data = nullptr;
		move._length = 0;
	}

	template<typename T>
	Buffer<T>::~Buffer() {
		deallocate();
	}

	template<typename T>
	void Buffer<T>::deallocate() {
		delete[] _data;
	}

	template<typename T>
	View<T> Buffer<T>::view(u64 offset, u64 length) const {
		AR_ASSERT(offset + length <= _length, "Offset + length is out of bounds for this buffer. (%zu + %zu >= %zu)", offset, length, _length);
		return View<T>{ _data + offset, length };
	}

	template<typename T>
	Slice<T> Buffer<T>::slice(u64 offset, u64 length) {
		AR_ASSERT(offset + length <= _length, "Offset + length is out of bounds for this buffer. (%zu + %zu >= %zu)", offset, length, _length);
		return Slice<T>{ _data + offset, length };
	}

}

#endif // AR_DATA_BUFFER_HH_
