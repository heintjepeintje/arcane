#include "./pipeline.hh"

#include <arcane/data/buffer.hh>

namespace arcane::graphics {

	GLenum get_opengl_shader_type(Shader::Type type) {
		switch (type) {
			case Shader::Type::VERTEX: return GL_VERTEX_SHADER;
			case Shader::Type::PIXEL: return GL_FRAGMENT_SHADER;
			default: return GL_NONE;
		}
	}

	UniquePtr<Shader> Shader::create(IAllocator& allocator, const SharedPtr<Context> &context, Type type, const View<u8> &binary) {
		AR_TRACE();
		UniquePtr<Shader> shader = UniquePtr<Shader>::create(allocator);
		shader.ptr()->_context = context.share();
		shader.ptr()->_context.ptr()->bind();

		shader.ptr()->_id = glCreateShader(get_opengl_shader_type(type));
		glShaderBinary(1, &shader.ptr()->_id, GL_SHADER_BINARY_FORMAT_SPIR_V, binary.data(), binary.length());
		glSpecializeShader(shader.ptr()->_id, "main", 0, nullptr, nullptr);

		return shader;
	}

	Shader::~Shader() {
		destroy();
	}

	void Shader::destroy() {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();

		glDeleteShader(_id);
		_id = 0;

		_context.release();
	}

	bool Shader::is_valid() const {
		if (!_context.is_valid()) return false;
		if (!_context.ptr()->is_valid()) return false;
		_context.ptr()->bind();

		return glIsShader(_id);
	}

	UniquePtr<Pipeline> Pipeline::create(IAllocator& allocator, const SharedPtr<Context>& context, const Info& info) {
		AR_TRACE();
		UniquePtr<Pipeline> pipeline = UniquePtr<Pipeline>::create(allocator);
		pipeline.ptr()->_context = context.share();
		pipeline.ptr()->_context.ptr()->bind();

		pipeline.ptr()->_program_id = glCreateProgram();
		if (info.vertex_shader != nullptr && info.vertex_shader->is_valid()) {
			AR_ASSERT(info.vertex_shader->context() == pipeline.ptr()->_context.ref(), "Vertex shader and pipeline must be from the same context.");
			glAttachShader(pipeline.ptr()->_program_id, info.vertex_shader->id());
		}

		if (info.fragment_shader != nullptr && info.fragment_shader->is_valid()) {
			AR_ASSERT(info.fragment_shader->context() == pipeline.ptr()->_context.ref(), "Fragment shader and pipeline must be from the same context.");
			glAttachShader(pipeline.ptr()->_program_id, info.fragment_shader->id());
		}

		glLinkProgram(pipeline.ptr()->_program_id);

		GLint status = 0;
		glGetProgramiv(pipeline.ptr()->_program_id, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint info_log_length = 0;
			glGetProgramiv(pipeline.ptr()->_program_id, GL_INFO_LOG_LENGTH, &info_log_length);

			arcane::Buffer<c8> info_log = arcane::Buffer<c8>::allocate(info_log_length + 1);
			glGetProgramInfoLog(pipeline.ptr()->_program_id, info_log.length(), nullptr, info_log.data());
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_ERROR, "%.*s", info_log_length, info_log.data());

			return UniquePtr<Pipeline>{};
		}

		pipeline.ptr()->_primitive_type = info.primitive_type;
		pipeline.ptr()->_winding_order = info.winding_order;
		pipeline.ptr()->_face_culling_mode = info.face_culling_mode;
		pipeline.ptr()->_viewport = info.viewport;
		pipeline.ptr()->_scissor = info.scissor;

		glValidateProgram(pipeline.ptr()->_program_id);

		return pipeline;
	}

	Pipeline::~Pipeline() {
		destroy();
	}

	void Pipeline::destroy() {
		if (!is_valid()) return;
		_context.ptr()->bind();
		glDeleteProgram(_program_id);
		_program_id = 0;

		_context.release();
	}

	bool Pipeline::is_valid() const {
		if (!_context.is_valid()) return false;
		if (!_context.ptr()->is_valid()) return false;
		_context.ptr()->bind();
		return glIsProgram(_program_id);
	}

}
