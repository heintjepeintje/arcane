#include "./window.hh"

#include <arcane/core/log.hh>

#include "GLFW/glfw3.h"
#include "arcane/system/memory.hh"
#include "minilog.h"

namespace arcane {

	bool Window::init() {
		if (!glfwInit()) {
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_FATAL, "Failed to initialize GLFW.");
			return false;
		}

		return true;
	}

	UniquePtr<Window> Window::create(IAllocator& allocator, const std::string& title, u32 width, u32 height) {
		UniquePtr<Window> window = UniquePtr<Window>::create(allocator);
		window.ptr()->_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (window.ptr()->_window == nullptr) {
			ml_log(AR_CORE_LOGGER_ID, ML_LOG_SEVERITY_ERROR, "Failed to create window.");
			return UniquePtr<Window>{};
		}

		return window;
	}

	Window::~Window() {
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
