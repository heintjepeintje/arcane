#include "./window.hh"

#include <arcane/core/log.hh>

#include "GLFW/glfw3.h"
#include "minilog.h"

namespace arcane {

	bool Window::init() {
		if (!glfwInit()) {
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_FATAL, "Failed to initialize GLFW.");
			return false;
		}

		return true;
	}

	Window Window::create(const std::string& title, u32 width, u32 height) {
		Window window;
		window._window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (window._window == nullptr) {
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_ERROR, "Failed to create window.");
			return Window();
		}

		return window;
	}


	Window::Window() : _window(nullptr) {
		AR_TRACE();
	}

	Window::Window(Window&& move) : _window(move._window) {
		AR_TRACE();
		move._window = nullptr;
	}
	
	Window& Window::operator=(Window&& move) {
		AR_TRACE();
		_window = move._window;
		move._window = nullptr;
		return *this;
	}

	Window::~Window() {
		AR_TRACE();
		destroy();
	}

	void Window::destroy() {
		if (_window == nullptr) return;
		AR_TRACE();
		glfwDestroyWindow(_window);
		_window = nullptr;
	}

	void Window::show() {
		if (_window == nullptr) return;
		AR_TRACE();
		glfwShowWindow(_window);
	}

	void Window::update() {
		if (_window == nullptr) return;
		AR_TRACE();
		glfwPollEvents();
	}
	
	bool Window::should_close() const {
		if (_window == nullptr) return true;
		AR_TRACE();
		return glfwWindowShouldClose(_window);
	}

	int32_t Window::width() const {
		int width, height;
		glfwGetWindowSize(_window, &width, &height);

		return width;
	}

	int32_t Window::height() const {
		int width, height;
		glfwGetWindowSize(_window, &width, &height);

		return height;
	}

	int32_t Window::x() const {
		int x, y;
		glfwGetWindowPos(_window, &x, &y);

		return x;
	}

	int32_t Window::y() const {
		int x, y;
		glfwGetWindowPos(_window, &x, &y);

		return y;
	}

	Rect2D Window::inner_rect() const {
		int width, height;
		glfwGetWindowSize(_window, &width, &height);

		return Rect2D{ { 0, 0 }, { static_cast<f32>(width), static_cast<f32>(height) } };
	}

}
