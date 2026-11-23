#ifndef AR_GRAPHICS_CONTEXT_HH_
#define AR_GRAPHICS_CONTEXT_HH_

#include <arcane/core/types.hh>
#include <arcane/system/memory.hh>
#include <arcane/system/window.hh>

namespace arcane::graphics {

	class Context {
	public:
		static UniquePtr<Context> create(IAllocator& allocator, const SharedPtr<Window>& window);

		Context() = default;
		~Context() = default;

		void unbind() const;
		void bind() const;
		void swap_buffers();

		bool operator==(const Context& context) const;

		inline const Window& window() const { return _window.ref(); }
		inline Window& window() { return _window.ref(); }
		inline bool is_valid() const { return _window.is_valid(); }

	private:
		SharedPtr<Window> _window;
	};

}

#endif // AR_GRAPHICS_RENDERER_HH_
