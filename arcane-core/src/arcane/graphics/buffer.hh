#ifndef AR_GRAPHICS_BUFFER_HH_
#define AR_GRAPHICS_BUFFER_HH_

#include <arcane/core/types.hh>
#include "./core.hh"
#include "./context.hh"
#include <arcane/data/view.hh>
#include <arcane/data/slice.hh>
#include <arcane/system/memory.hh>

namespace arcane::graphics {

	enum BufferFlagBits {
		AR_BUFFER_FLAG_DYNAMIC_STORAGE = (1 << 0),
		AR_BUFFER_FLAG_MAP_READABLE = (1 << 1),
		AR_BUFFER_FLAG_MAP_WRITEABLE = (1 << 2),
	};

	using BufferFlags = u32;

	class Buffer : public Object {
	public:
		enum class Type {
			NONE = 0,
			VERTEX,
			INDEX,
			UNIFORM,
			SHADER_STORAGE
		};

		static UniquePtr<Buffer> create(IAllocator& allocator, const SharedPtr<Context>& context, Type type, u64 size, BufferFlags flags);
		static void copy(const Buffer& input, Buffer& output, u64 read_offset, u64 write_offset, u64 length);

		~Buffer();

		View<u8> map(u64 offset, u64 length) const;
		Slice<u8> map(u64 offset, u64 length);

		inline View<u8> map() const { return map(0, _size); }
		inline Slice<u8> map() { return map(0, _size); }

		void unmap();

		void destroy() override;
		bool is_valid() const override;

		inline GLuint id() const { return _id; }
		inline u64 size() const { return _size; }

		inline Context& context() override { return _context.ref(); }
		inline const Context& context() const override { return _context.ref(); }

	private:
	 	SharedPtr<Context> _context;
		GLuint _id;
		u64 _size;
	};

}

#endif // AR_GRAPHICS_BUFFER_HH_
