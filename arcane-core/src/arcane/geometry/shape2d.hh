#ifndef AR_GEOMETRY_SHAPE2D_HH_
#define AR_GEOMETRY_SHAPE2D_HH_

#include <arcane/core/types.hh>
#include <arcane/math/vector.hh>

namespace arcane {

	struct Rect2D {
		glm::vec2 position;
		glm::vec2 size;
	};

}

#endif // AR_GEOMETRY_SHAPE2D_HH_
