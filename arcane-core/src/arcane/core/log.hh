#ifndef AR_CORE_LOG_HH_
#define AR_CORE_LOG_HH_

#include <minilog.h>

#ifndef AR_CORE_LOGGER_ID
#	define AR_CORE_LOGGER_ID 0
#endif

#ifndef AR_DISABLE_LOGGING
#define AR_LOG_TRACE(...) ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_TRACE, __VA_ARGS__)
#define AR_LOG_DEBUG(...) ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define AR_LOG_INFO(...) ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_INFO, __VA_ARGS__)
#define AR_LOG_WARNING(...) ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_WARNING, __VA_ARGS__)
#define AR_LOG_ERROR(...) ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_ERROR, __VA_ARGS__)
#define AR_LOG_FATAL(...) ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_FATAL, __VA_ARGS__)
#else
#define AR_LOG_TRACE(...)
#define AR_LOG_DEBUG(...)
#define AR_LOG_INFO(...)
#define AR_LOG_WARNING(...)
#define AR_LOG_ERROR(...)
#define AR_LOG_FATAL(...)
#endif

#define AR_TRACE() AR_LOG_TRACE("%s:%u: %s", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define AR_ASSERT(expr, ...) if (!(expr)) { AR_LOG_ERROR(__VA_ARGS__); }

#endif // AR_CORE_LOG_HH_
