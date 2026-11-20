#include "./renderer.hh"
#include "arcane/graphics/core.hh"

namespace arcane::graphics {

	GLenum to_opengl_primitive_type(Pipeline::PrimitiveType primitive_type) {
		switch (primitive_type) {
			case Pipeline::PrimitiveType::POINT_LIST: return GL_POINTS;
			case Pipeline::PrimitiveType::LINE_STRIP: return GL_LINE_STRIP;
			case Pipeline::PrimitiveType::LINE_LOOP: return GL_LINE_LOOP;
			case Pipeline::PrimitiveType::LINE_LIST: return GL_LINES;
			case Pipeline::PrimitiveType::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
			case Pipeline::PrimitiveType::TRIANGLE_FAN: return GL_TRIANGLE_FAN;
			case Pipeline::PrimitiveType::TRIANGLE_LIST: return GL_TRIANGLES;
			default: return GL_NONE;
		}
	}

	GLenum to_opengl_front_face(Pipeline::WindingOrder winding_order) {
		switch (winding_order) {
			case Pipeline::WindingOrder::CLOCKWISE: return GL_CW;	
			case Pipeline::WindingOrder::COUNTER_CLOCKWISE: return GL_CCW;	
			default: return GL_NONE;
		}
	}
	
	GLenum to_opengl_cull_face(Pipeline::FaceCullingMode face_culling_mode) {
		switch (face_culling_mode) {
			case Pipeline::FaceCullingMode::FRONT: return GL_FRONT;
			case Pipeline::FaceCullingMode::BACK: return GL_BACK;
			case Pipeline::FaceCullingMode::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
			default: return GL_NONE;
		}
	}

	Renderer Renderer::create(Context& context) {
		AR_TRACE();
		Renderer renderer{};
		renderer._context = &context;

		return renderer;
	}

	Renderer::Renderer() { }

	Renderer::~Renderer() { }

	void Renderer::begin() {
		AR_TRACE();
	}

	void Renderer::clear() {
		AR_TRACE();
		_context->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::set_mesh(const Mesh& mesh) {
		AR_ASSERT(mesh.is_valid(), "Mesh is not valid.");
		AR_ASSERT(mesh.context() == *_context, "Mesh and renderer are not from the same context.");
		AR_TRACE();
		_context->bind();
		_current_mesh = &mesh;
		glBindVertexArray(mesh.id());
	}

	void Renderer::set_pipeline(const Pipeline& pipeline) {
		AR_ASSERT(pipeline.is_valid(), "Pipeline is not valid.");
		AR_ASSERT(pipeline.context() == *_context, "Pipeline and renderer are not from the same context.");
		AR_TRACE();

		_current_pipeline = &pipeline;
		_context->bind();

		glFrontFace(to_opengl_front_face(pipeline.winding_order()));
		glCullFace(to_opengl_cull_face(pipeline.face_culling_mode()));
		glViewport(pipeline.viewport().position.x, pipeline.viewport().position.y, pipeline.viewport().size.x, pipeline.viewport().size.y);
		glScissor(pipeline.scissor().position.x, pipeline.scissor().position.y, pipeline.scissor().size.x, pipeline.scissor().size.y);
		glUseProgram(pipeline.program_id());
	}

	void Renderer::set_viewport(const Rect2D& viewport) {
		AR_TRACE();
		_context->bind();
		glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
	}

	void Renderer::set_scissor(const Rect2D& scissor) {
		AR_TRACE();
		_context->bind();
		glScissor(scissor.position.x, scissor.position.y, scissor.size.x, scissor.size.y);
	}

	void Renderer::draw_indexed(u32 instance_count, u32 index_count) {
		AR_TRACE();
		_context->bind();
		glDrawElementsInstanced(to_opengl_primitive_type(_current_pipeline->primitive_type()), index_count, GL_UNSIGNED_INT, nullptr, instance_count);
	}

	void Renderer::end() {
		AR_TRACE();
		_context->bind();
	}

}
