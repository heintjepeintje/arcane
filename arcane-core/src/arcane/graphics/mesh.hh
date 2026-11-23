#ifndef AR_GRAPHICS_MESH_HH_
#define AR_GRAPHICS_MESH_HH_

#include <arcane/core/types.hh>
#include "./core.hh"
#include "./buffer.hh"
#include "./context.hh"
#include "./pipeline.hh"
#include <vector>

namespace arcane::graphics {

	class Mesh : public Object {
	public:
		static UniquePtr<Mesh> create(IAllocator& allocator, const SharedPtr<Context>& context);

		~Mesh();

		void destroy() override;
		bool is_valid() const override;

		void set_vertex_buffer(u32 index, const SharedPtr<Buffer>& buffer, u64 offset, u64 stride);
		void set_index_buffer(const SharedPtr<Buffer>& buffer);
		void set_layout(const std::vector<InputElement>& layout);

		inline const Context& context() const override { return _context.ref(); }
		inline Context& context() override { return _context.ref(); }
		inline GLuint id() const { return _id; }

	private:
		SharedPtr<Context> _context;
		GLuint _id;
		std::vector<SharedPtr<Buffer>> _vertex_buffers;
		SharedPtr<Buffer> _index_buffer;
		std::vector<InputElement> _layout;
	};

}

#endif // AR_GRAPHICS_MESH_HH_
