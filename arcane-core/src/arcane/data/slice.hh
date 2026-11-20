#ifndef AR_DATA_STRIDE_HH_
#define AR_DATA_STRIDE_HH_

#include <arcane/core/types.hh>
#include <arcane/core/log.hh>

namespace arcane {

	template<typename T = u8>
	class Slice {
	public:
		Slice();
		Slice(T* data, u64 length);
		Slice(T* begin, T* end);

		template<typename U>
		Slice<U> as() const;

		inline T& at(u64 index) const { return _data[index]; }

		inline T* begin() const { return _data; }
		inline T* end() const { return _data + _length; }
		inline u64 length() const { return _length; }

	private:
		T* _data;
		u64 _length;
	};

	template<typename T>
	Slice<T>::Slice() : _data(nullptr), _length(0) { }

	template<typename T>
	Slice<T>::Slice(T* data, u64 length) : _data(data), _length(length) { }

	template<typename T>
	Slice<T>::Slice(T* begin, T* end) : _data(begin), _length(end - begin) { }

	template<typename T>
	template<typename U>
	Slice<U> Slice<T>::as() const {
		AR_ASSERT((_length * sizeof(T)) % sizeof(U) == 0, "Slice of T cannot be converted to view of U.");
		return Slice<U>{ reinterpret_cast<U*>(_data), (_length * sizeof(T)) / sizeof(U) };
	}

}

#endif // AR_DATA_STRIDE_HH_
