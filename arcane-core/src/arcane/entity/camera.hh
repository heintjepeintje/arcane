#ifndef AR_ENTITY_CAMERA_HH_
#define AR_ENTITY_CAMERA_HH_

#include <arcane/core/types.hh>
#include <arcane/math/vector.hh>

namespace arcane {

	class Camera {
	public:
		virtual ~Camera() = default;

		virtual glm::mat4 projection() const = 0;
		virtual glm::mat4 view() const = 0;
	};

	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera();
		OrthographicCamera(const glm::vec3& position);

		glm::mat4 projection() const override;
		glm::mat4 view() const override;

		glm::vec3 position;
		f32 left_plane, right_plane;
		f32 top_plane, bottom_plane;
		f32 near_plane, far_plane;
	};

	class PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera();
		PerspectiveCamera(const glm::vec3& position);

		glm::mat4 projection() const override;
		glm::mat4 view() const override;

		glm::vec3 position;
		glm::vec3 direction;
		f32 field_of_view;
		f32 aspect_ratio;
		f32 near_plane, far_plane;
	};

}

#endif // AR_ENTITY_CAMERA_HH_
