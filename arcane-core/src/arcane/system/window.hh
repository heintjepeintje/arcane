#ifndef AR_SYSTEM_WINDOW_HH_
#define AR_SYSTEM_WINDOW_HH_

#include <arcane/core/types.hh>
#include <arcane/system/memory.hh>
#include <arcane/geometry/shape2d.hh>
#include <GLFW/glfw3.h>
#include <string>

namespace arcane {

	class Window {
	public:
		static bool init();
		static UniquePtr<Window> create(IAllocator& allocator, const std::string& title, u32 width, u32 height);

		~Window();

		void destroy();
		inline bool is_valid() const { return _window != NULL; }

		void show();
		void update();

		bool should_close() const;

		int32_t width() const;
		int32_t height() const;
		int32_t x() const;
		int32_t y() const;

		Rect2D inner_rect() const;

		inline GLFWwindow* handle() { return _window; }
	private:
		GLFWwindow* _window;
	};

}

#endif // AR_SYSTEM_WINDOW_HH_
