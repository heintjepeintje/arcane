#ifndef AR_GRAPHICS_CORE_HH_
#define AR_GRAPHICS_CORE_HH_

#include <glad/glad.h>
#include "./context.hh"

namespace arcane::graphics {

	class Object {
	public:
		virtual ~Object() = default;

		virtual void destroy() = 0;
		virtual bool is_valid() const = 0;
		virtual Context& context() = 0;
		virtual const Context& context() const = 0;
	};

}

#endif // AR_GRAPHICS_CORE_HH_
