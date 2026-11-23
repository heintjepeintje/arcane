#ifndef AR_ENTITY_TRANSFORM_HH_
#define AR_ENTITY_TRANSFORM_HH_

#include <arcane/core/types.hh>
#include <glm/glm.hpp>

namespace arcane {

	class Transform {
	public:
		virtual ~Transform() = default;

		virtual glm::mat4 model() const = 0;
	};

	class Transform3D : public Transform {
	public:
		Transform3D();
		Transform3D(const glm::vec3& position);
		Transform3D(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

		glm::mat4 model() const override;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};

}

#endif // AR_ENTITY_TRANSFORM_HH_
