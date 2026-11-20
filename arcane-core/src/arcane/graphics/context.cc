#include "./core.hh"
#include "./context.hh"
#include <GLFW/glfw3.h>

#include <arcane/core/log.hh>

namespace arcane::graphics {

	void opengl_debug_message_callback(GLenum source, GLenum type, GLenum id, GLenum severity, GLsizei length, const GLchar* message, const void* data) {
		switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION: {
				ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_INFO, "%.*s", length, message, severity);
			} break;

			case GL_DEBUG_SEVERITY_LOW: {
				ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_WARNING, "%.*s", length, message, severity);
			} break;

			case GL_DEBUG_SEVERITY_MEDIUM: {
				ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_ERROR, "%.*s", length, message, severity);
			} break;

			case GL_DEBUG_SEVERITY_HIGH: {
				ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_FATAL, "%.*s", length, message, severity);
			} break;
		}
	}

	Context Context::create(Window& window) {
		AR_TRACE();
		Context context{};
		context._window = &window;

		GLFWwindow* old_context = glfwGetCurrentContext();
		glfwMakeContextCurrent(context._window->handle());
		if (!gladLoadGL()) {
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_FATAL, "Failed to load OpenGL functions.");
			return Context{};
		}

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_SCISSOR_TEST);

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(opengl_debug_message_callback, nullptr);

		glfwMakeContextCurrent(old_context);

		return context;
	}

	Context::Context() : _window(nullptr) { }

	Context::~Context() { _window = nullptr; }

	void Context::bind() {
		glfwMakeContextCurrent(_window->handle());
	}

	void Context::unbind() {
		if (glfwGetCurrentContext() != _window->handle()) return;
		AR_TRACE();
		glfwMakeContextCurrent(nullptr);
	}

	void Context::swap_buffers() {
		AR_TRACE();
		glfwSwapBuffers(_window->handle());
	}

	bool Context::operator==(const Context& context) const {
		return _window->handle() == context._window->handle();
	}

}
