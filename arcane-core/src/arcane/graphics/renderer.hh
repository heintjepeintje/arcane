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
		static UniquePtr<Renderer> create(IAllocator& allocator, const SharedPtr<Context>& context);

		~Renderer();

		void destroy();
		bool is_valid() const;

		void begin();
		void clear();
		void set_mesh(const SharedPtr<Mesh>& mesh);
		void set_pipeline(const SharedPtr<Pipeline>& pipeline);
		void set_viewport(const Rect2D& viewport);
		void set_scissor(const Rect2D& scissor);
		void set_texture_sampler(u32 index, const SharedPtr<Texture>& texture, const SharedPtr<Sampler>& sampler);
		void set_uniform_buffer(u32 index, const SharedPtr<Buffer>& buffer);
		void draw_indexed(u32 instance_count, u32 index_count);
		void end();

		inline const Mesh* current_mesh() const { return _current_mesh.ptr(); }
		inline const Pipeline* current_pipeline() const { return _current_pipeline.ptr(); }

		inline const Context& context() const { return _context.ref(); } 
		inline Context& context() { return _context.ref(); } 

	private:
		SharedPtr<Context> _context;
		SharedPtr<Mesh> _current_mesh;
		SharedPtr<Pipeline> _current_pipeline;

		struct TextureSampler {
			SharedPtr<Texture> texture;
			SharedPtr<Sampler> sampler;
		};

		std::vector<TextureSampler> _texture_samplers;
		std::vector<SharedPtr<Buffer>> _uniform_buffers;
	};

}

#endif // AR_GRAPHICS_RENDERER_HH_
