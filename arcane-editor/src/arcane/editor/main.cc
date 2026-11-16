#include <arcane/core/platform.hh>
#include <arcane/core/types.hh>
#include <arcane/core/version.hh>
#include <minilog.h>

#include <cstdio>

#define DEFAULT_LOGGER 0

int main(int argc, char** argv) {
	ml_create_logger("EDITOR", ML_SEVERITY_FLAGS_ALL, FILE *file, ml_logger_id_t logger_id)
#ifdef AR_PLATFORM_COMPILER_GNU
	std::printf("Compiler: GNU (v%u.%u.%u.%u)\n",
			static_cast<arcane::u32>(arcane::compiler_version.variant),
			static_cast<arcane::u32>(arcane::compiler_version.major),
			static_cast<arcane::u32>(arcane::compiler_version.minor),
			static_cast<arcane::u32>(arcane::compiler_version.patch));
#endif
	return 0;
}
