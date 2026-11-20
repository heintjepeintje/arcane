#ifndef AR_DATA_VIEW_HH_
#define AR_DATA_VIEW_HH_

#include <arcane/core/types.hh>
#include <arcane/core/log.hh>

namespace arcane {

	template<typename T = u8>
	class View {
	public:
		View();
		View(const T* data, u64 length);
		View(const T* begin, const T* end);

		template<typename U>
		View<U> as() const;

		const T& at(u64 index) const { return _data[index]; }

		inline const T* data() const { return _data; }
		inline const T* begin() const { return _data; }
		inline const T* end() const { return _data + _length; }
		inline u64 length() const { return _length; }

	private:
		const T* _data;
		u64 _length;
	};

	template<typename T>
	View<T>::View() : _data(nullptr), _length(0) { }

	template<typename T>
	View<T>::View(const T* data, u64 length) : _data(data), _length(length) { }

	template<typename T>
	View<T>::View(const T* begin, const T* end) : _data(begin), _length(end - begin) { }

	template<typename T>
	template<typename U>
	View<U> View<T>::as() const {
		AR_ASSERT((_length * sizeof(T)) % sizeof(U) == 0, "View of T cannot be converted to view of U.");
		return View<U>{ reinterpret_cast<const U*>(_data), (_length * sizeof(T)) / sizeof(U) };
	}

}

#endif // AR_DATA_VIEW_HH_
