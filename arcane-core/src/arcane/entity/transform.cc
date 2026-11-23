#include "./transform.hh"

#include <glm/ext/matrix_transform.hpp>

namespace arcane {

	Transform3D::Transform3D() : position(), rotation(), scale(1.0f) { }

	Transform3D::Transform3D(const glm::vec3& position) : position(position), rotation(), scale(1.0f) { }
	Transform3D::Transform3D(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : position(position), rotation(rotation), scale(scale) { }

	glm::mat4 Transform3D::model() const {
		glm::mat4 model = glm::identity<glm::mat4>();
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.x, { 1.0f, 0.0f, 0.0f });
		model = glm::rotate(model, rotation.y, { 0.0f, 1.0f, 0.0f });
		model = glm::rotate(model, rotation.z, { 0.0f, 0.0f, 1.0f });
		model = glm::scale(model, scale);

		return model;
	}

}
