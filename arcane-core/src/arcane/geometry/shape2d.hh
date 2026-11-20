#ifndef AR_GEOMETRY_SHAPE2D_HH_
#define AR_GEOMETRY_SHAPE2D_HH_

#include <arcane/core/types.hh>
#include <minivector.h>

namespace arcane {

	struct Rect2D {
		mm_vec2_t position;
		mm_vec2_t size;
	};

}

#endif // AR_GEOMETRY_SHAPE2D_HH_
