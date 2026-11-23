#include "./camera.hh"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_projection.hpp>

namespace arcane {

	OrthographicCamera::OrthographicCamera() { }

	OrthographicCamera::OrthographicCamera(const glm::vec3& position) : position(position) { }

	glm::mat4 OrthographicCamera::projection() const {
		return glm::orthoRH(left_plane, right_plane, bottom_plane, top_plane, near_plane, far_plane);
	}

	glm::mat4 OrthographicCamera::view() const {
		return glm::translate(glm::identity<glm::mat4>(), position);
	}

	PerspectiveCamera::PerspectiveCamera() { }

	PerspectiveCamera::PerspectiveCamera(const glm::vec3& position) : position(position) { }

	glm::mat4 PerspectiveCamera::projection() const {
		return glm::perspectiveRH(field_of_view, aspect_ratio, near_plane, far_plane);
	}

	glm::mat4 PerspectiveCamera::view() const {
		return glm::lookAtRH(position, position + glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f));
	}

}
