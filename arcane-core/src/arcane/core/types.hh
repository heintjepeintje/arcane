#ifndef AR_CORE_TYPES_HH_
#define AR_CORE_TYPES_HH_

#include "./platform.hh"

namespace arcane {

	using i8 = AR_COMPILER_INT8_TYPE;
	using i16 = AR_COMPILER_INT16_TYPE;
	using i32 = AR_COMPILER_INT32_TYPE;
	using i64 = AR_COMPILER_INT64_TYPE;

	using u8 = AR_COMPILER_UINT8_TYPE;
	using u16 = AR_COMPILER_UINT16_TYPE;
	using u32 = AR_COMPILER_UINT32_TYPE;
	using u64 = AR_COMPILER_UINT64_TYPE;

	using f32 = AR_COMPILER_FLOAT32_TYPE;
	using f64 = AR_COMPILER_FLOAT64_TYPE;

	using c8 = AR_COMPILER_CHAR8_TYPE;
	using c16 = AR_COMPILER_CHAR16_TYPE;

	template<typename T> class is_numeric_type { public: constexpr static bool value = false; };
	template<> class is_numeric_type<i8> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<i16> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<i32> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<i64> { public: constexpr static bool value = true; };

	template<> class is_numeric_type<u8> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<u16> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<u32> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<u64> { public: constexpr static bool value = true; };

	template<> class is_numeric_type<f32> { public: constexpr static bool value = true; };
	template<> class is_numeric_type<f64> { public: constexpr static bool value = true; };

	template<typename T> class is_floating_point_type { public: constexpr static bool value = false; };
	template<> class is_floating_point_type<f32> { public: constexpr static bool value = true; };
	template<> class is_floating_point_type<f64> { public: constexpr static bool value = true; };

	template<typename T> class is_character_type { public: constexpr static bool value = false; };
	template<> class is_character_type<c8> { public: constexpr static bool value = true; };
	template<> class is_character_type<c16> { public: constexpr static bool value = true; };

	template<typename T> class limits { public: constexpr static T max_value = 0; };
	template<> class limits<i8> { public: constexpr static i8 max_value = AR_COMPILER_INT8_MAX_VALUE; };
	template<> class limits<i16> { public: constexpr static i16 max_value = AR_COMPILER_INT16_MAX_VALUE; };
	template<> class limits<i32> { public: constexpr static i32 max_value = AR_COMPILER_INT32_MAX_VALUE; };
	template<> class limits<i64> { public: constexpr static i64 max_value = AR_COMPILER_INT64_MAX_VALUE; };

	template<> class limits<u8> { public: constexpr static u8 max_value = AR_COMPILER_UINT8_MAX_VALUE; };
	template<> class limits<u16> { public: constexpr static u16 max_value = AR_COMPILER_UINT16_MAX_VALUE; };
	template<> class limits<u32> { public: constexpr static u32 max_value = AR_COMPILER_UINT32_MAX_VALUE; };
	template<> class limits<u64> { public: constexpr static u64 max_value = AR_COMPILER_UINT64_MAX_VALUE; };

	template<> class limits<f32> { public: constexpr static f32 max_value = AR_COMPILER_FLOAT32_MAX_VALUE; };
	template<> class limits<f64> { public: constexpr static f64 max_value = AR_COMPILER_FLOAT64_MAX_VALUE; };

}

#endif // AR_CORE_TYPES_HH_
