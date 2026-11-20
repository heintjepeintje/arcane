#ifndef AR_GRAPHICS_RENDERER_HH_
#define AR_GRAPHICS_RENDERER_HH_

#include <arcane/core/types.hh>
#include "./mesh.hh"
#include "./pipeline.hh"
#include "./texture.hh"
#include "./context.hh"

namespace arcane::graphics {

	class Renderer {
	public:
		static Renderer create(Context& context);

		Renderer();
		~Renderer();

		void begin();
		void clear();
		void set_mesh(const Mesh& mesh);
		void set_pipeline(const Pipeline& pipeline);
		void set_viewport(const Rect2D& viewport);
		void set_scissor(const Rect2D& scissor);
		void set_texture_sampler(u32 index, const Texture& texture, const Sampler& sampler);
		void draw_indexed(u32 instance_count, u32 index_count);
		void end();

		inline const Mesh* current_mesh() const { return _current_mesh; }
		inline const Pipeline* current_pipeline() const { return _current_pipeline; }

		inline const Context& context() const { return *_context; } 
		inline Context& context() { return *_context; } 

	private:
		Context* _context;
		const Mesh* _current_mesh;
		const Pipeline* _current_pipeline;
	};

}

#endif // AR_GRAPHICS_RENDERER_HH_
