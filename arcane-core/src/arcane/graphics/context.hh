#ifndef AR_GRAPHICS_CONTEXT_HH_
#define AR_GRAPHICS_CONTEXT_HH_

#include <arcane/system/window.hh>

namespace arcane::graphics {

	class Context {
	public:
		static Context create(Window& window);

		Context();
		~Context();

		void unbind();
		void bind();
		void swap_buffers();

		bool operator==(const Context& context) const;

		inline const Window* window() const { return _window; }
		inline Window* window() { return _window; }
		inline bool is_valid() const { return _window != nullptr; }

	private:
		Window* _window;
	};

}

#endif // AR_GRAPHICS_RENDERER_HH_
