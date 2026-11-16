#ifndef AR_CORE_VERSION_HH_
#define AR_CORE_VERSION_HH_

#include "./platform.hh"
#include "./types.hh"

namespace arcane {

	struct Version {
		u16 variant, major, minor, patch;
	};

	constexpr inline Version compiler_version = {
		AR_PLATFORM_COMPILER_VERSION_VARIANT,
		AR_PLATFORM_COMPILER_VERSION_MAJOR,
		AR_PLATFORM_COMPILER_VERSION_MINOR,
		AR_PLATFORM_COMPILER_VERSION_PATCHLEVEL
	};

}

#endif // AR_CORE_VERSION_HH_
