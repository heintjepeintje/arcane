#ifndef AR_GRAPHICS_FRAMEBUFFER_HH_
#define AR_GRAPHICS_FRAMEBUFFER_HH_

#include <arcane/core/types.hh>
#include "./texture.hh"
#include "./context.hh"
#include <vector>

namespace arcane::graphics {

	class Framebuffer : public Object {
	public:
		struct Attachment {
			ImageFormat format;	
		};

		struct Info {
			u32 width, height;
			std::vector<Attachment> attachments;
		};

		static Framebuffer create(Context& context, const Info& info);

		Framebuffer();
		Framebuffer(const Framebuffer& framebuffer) = delete;
		Framebuffer& operator=(const Framebuffer& framebuffer) = delete;
		Framebuffer(Framebuffer&& framebuffer);
		Framebuffer& operator=(Framebuffer&& framebuffer);
		~Framebuffer();

		void destroy() override;
		bool is_valid() const override;

		inline Context& context() override { return *_context; }
		inline const Context& context() const override { return *_context; }

	private:
		Context* _context;
		GLuint _id;
	};

}

#endif // AR_GRAPHICS_FRAMEBUFFER_HH_
