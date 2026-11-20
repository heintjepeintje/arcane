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

	Shader Shader::create(Context &context, Type type, const View<u8> &binary) {
		AR_TRACE();
		Shader shader{};
		shader._context = &context;
		shader._context->bind();

		shader._id = glCreateShader(get_opengl_shader_type(type));
		glShaderBinary(1, &shader._id, GL_SHADER_BINARY_FORMAT_SPIR_V, binary.data(), binary.length());
		glSpecializeShader(shader._id, "main", 0, nullptr, nullptr);

		return shader;
	}

	Shader::Shader() { }

	Shader::Shader(Shader&& move) : _context(move._context), _type(move._type), _id(move._id) {

	}

	Shader& Shader::operator=(Shader&& move) {
		_context = move._context;
		_type = move._type;
		_id = move._id;

		move._context = nullptr;
		move._type = static_cast<Type>(0);
		move._id = 0;

		return *this;
	}

	Shader::~Shader() {
		destroy();
	}

	void Shader::destroy() {
		if (!is_valid()) return;
		AR_TRACE();
		_context->bind();

		glDeleteShader(_id);
		_context = nullptr;
		_id = 0;
	}

	bool Shader::is_valid() const {
		if (_context == nullptr) return false;
		_context->bind();
		return glIsShader(_id);
	}

	Pipeline Pipeline::create(Context& context, const Info& info) {
		AR_TRACE();
		Pipeline pipeline{};
		pipeline._context = &context;

		pipeline._context->bind();

		pipeline._program_id = glCreateProgram();
		if (info.vertex_shader.is_valid()) {
			AR_ASSERT(info.vertex_shader.context() == *pipeline._context, "Vertex shader and pipeline must be from the same context.");
			glAttachShader(pipeline._program_id, info.vertex_shader.id());
		}

		if (info.fragment_shader.is_valid()) {
			AR_ASSERT(info.fragment_shader.context() == *pipeline._context, "Fragment shader and pipeline must be from the same context.");
			glAttachShader(pipeline._program_id, info.fragment_shader.id());
		}

		glLinkProgram(pipeline._program_id);

		GLint status = 0;
		glGetProgramiv(pipeline._program_id, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint info_log_length = 0;
			glGetProgramiv(pipeline._program_id, GL_INFO_LOG_LENGTH, &info_log_length);

			arcane::Buffer<c8> info_log = arcane::Buffer<c8>::allocate(info_log_length + 1);
			glGetProgramInfoLog(pipeline._program_id, info_log.length(), nullptr, info_log.data());
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_ERROR, "%.*s", info_log_length, info_log.data());

			return Pipeline{};
		}

		pipeline._data.primitive_type = info.primitive_type;
		pipeline._data.winding_order = info.winding_order;
		pipeline._data.face_culling_mode = info.face_culling_mode;
		pipeline._data.viewport = info.viewport;
		pipeline._data.scissor = info.scissor;

		glValidateProgram(pipeline._program_id);

		return pipeline;
	}

	Pipeline::Pipeline() { }

	Pipeline::Pipeline(Pipeline&& move) : _context(move._context), _program_id(move._program_id), _data(move._data) {
		move._context = nullptr;
		move._program_id = 0;
		std::memset(&move._data, 0, sizeof(Data));
	}

	Pipeline& Pipeline::operator=(Pipeline&& move) {
		_context = move._context;
		_program_id = move._program_id;
		_data = move._data;

		move._context = nullptr;
		move._program_id = 0;
		std::memset(&move._data, 0, sizeof(Data));

		return *this;
	}

	Pipeline::~Pipeline() {
		destroy();
	}

	void Pipeline::destroy() {
		if (!is_valid()) return;
		_context->bind();
		glDeleteProgram(_program_id);
		_program_id = 0;
	}

	bool Pipeline::is_valid() const {
		if (_context == nullptr) return false;
		_context->bind();
		return glIsProgram(_program_id);
	}

}
