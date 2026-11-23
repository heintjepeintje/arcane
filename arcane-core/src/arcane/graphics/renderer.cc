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

	UniquePtr<Renderer> Renderer::create(IAllocator& allocator, const SharedPtr<Context>& context) {
		AR_TRACE();
		UniquePtr<Renderer> renderer = UniquePtr<Renderer>::create(allocator);
		renderer.ptr()->_context = context.share();

		return renderer;
	}

	Renderer::~Renderer() {
		destroy();
	}

	void Renderer::destroy() {
		_context.ptr()->bind();

		for (TextureSampler& texture_sampler : _texture_samplers) {
			texture_sampler.texture.release();
			texture_sampler.sampler.release();
		}

		for (SharedPtr<Buffer>& uniform_buffer : _uniform_buffers) {
			uniform_buffer.release();
		}

		_current_pipeline.release();

		_current_mesh.release();

		_context.release();
	}

	bool Renderer::is_valid() const {
		return _context.is_valid() && _context.ptr()->is_valid();
	}

	void Renderer::begin() {
		AR_TRACE();
	}

	void Renderer::clear() {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::set_mesh(const SharedPtr<Mesh>& mesh) {
		AR_ASSERT(mesh.is_valid() && mesh.ptr()->is_valid(), "Mesh is not valid.");
		AR_ASSERT(mesh.ptr()->context() == _context.ref(), "Mesh and renderer are not from the same context.");
		AR_TRACE();
		_context.ptr()->bind();

		_current_mesh = mesh.share();

		glBindVertexArray(_current_mesh.ptr()->id());
	}

	void Renderer::set_pipeline(const SharedPtr<Pipeline>& pipeline) {
		AR_ASSERT(pipeline.is_valid(), "Pipeline is not valid.");
		AR_ASSERT(pipeline.ptr()->context() == _context.ref(), "Pipeline and renderer are not from the same context.");
		AR_TRACE();

		_current_pipeline = pipeline.share();
		_context.ptr()->bind();

		glFrontFace(to_opengl_front_face(pipeline.ptr()->winding_order()));
		glCullFace(to_opengl_cull_face(pipeline.ptr()->face_culling_mode()));
		glViewport(
				pipeline.ptr()->viewport().position.x, pipeline.ptr()->viewport().position.y, 
				pipeline.ptr()->viewport().size.x, pipeline.ptr()->viewport().size.y);

		glScissor(
				pipeline.ptr()->scissor().position.x, pipeline.ptr()->scissor().position.y,
				pipeline.ptr()->scissor().size.x, pipeline.ptr()->scissor().size.y);

		glUseProgram(pipeline.ptr()->program_id());
	}

	void Renderer::set_viewport(const Rect2D& viewport) {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();

		glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
	}

	void Renderer::set_scissor(const Rect2D& scissor) {
		if (!is_valid()) return;

		AR_TRACE();
		_context.ptr()->bind();

		glScissor(scissor.position.x, scissor.position.y, scissor.size.x, scissor.size.y);
	}

	void Renderer::set_texture_sampler(u32 index, const SharedPtr<Texture>& texture, const SharedPtr<Sampler>& sampler) {
		if (!is_valid()) return;
		if (!texture.is_valid() || !texture.ptr()->is_valid()) return;
		if (!sampler.is_valid() || !sampler.ptr()->is_valid()) return;

		AR_TRACE();
		_context.ptr()->bind();

		_uniform_buffers.resize(index + 1);
		_texture_samplers.emplace(_texture_samplers.begin() + index, TextureSampler{ texture.share(), sampler.share() });

		glBindTextureUnit(index, texture.ptr()->id());
		glBindSampler(index, sampler.ptr()->id());
	}

	void Renderer::set_uniform_buffer(u32 index, const SharedPtr<Buffer>& buffer) {
		if (!is_valid()) return;
		if (!buffer.is_valid() || !buffer.ptr()->is_valid()) return;

		AR_TRACE();
		_context.ptr()->bind();

		_uniform_buffers.resize(index + 1);
		_uniform_buffers.emplace(_uniform_buffers.begin() + index, buffer.share());

		glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer.ptr()->id());
	}

	void Renderer::draw_indexed(u32 instance_count, u32 index_count) {
		if (!is_valid()) return;

		AR_TRACE();
		_context.ptr()->bind();
		glDrawElementsInstanced(
				to_opengl_primitive_type(_current_pipeline.ptr()->primitive_type()), 
				index_count,
				GL_UNSIGNED_INT,
				nullptr,
				instance_count);
	}

	void Renderer::end() {
		if (!is_valid()) return;
		AR_TRACE();

		_context.ptr()->bind();

		for (TextureSampler& texture_sampler : _texture_samplers) {
			texture_sampler.texture.release();
			texture_sampler.sampler.release();
		}

		for (SharedPtr<Buffer>& uniform_buffer : _uniform_buffers) {
			uniform_buffer.release();
		}

		_current_pipeline.release();

		_current_mesh.release();
	}

}
