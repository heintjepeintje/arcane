#ifndef AR_GRAPHICS_BUFFER_HH_
#define AR_GRAPHICS_BUFFER_HH_

#include <arcane/core/types.hh>
#include "./core.hh"
#include "./context.hh"
#include <arcane/data/view.hh>
#include <arcane/data/slice.hh>

namespace arcane::graphics {

	enum class BufferType {
		NONE = 0,
		VERTEX,
		INDEX,
		UNIFORM,
		SHADER_STORAGE
	};

	enum BufferFlagBits {
		AR_BUFFER_FLAG_DYNAMIC_STORAGE = (1 << 0),
		AR_BUFFER_FLAG_MAP_READABLE = (1 << 1),
		AR_BUFFER_FLAG_MAP_WRITEABLE = (1 << 2),
	};

	using BufferFlags = u32;

	enum BufferAccessFlagBits {
		AR_BUFFER_ACCESS_FLAG_READ_BIT = (1 << 0),
		AR_BUFFER_ACCESS_FLAG_WRITE_BIT = (1 << 1)
	};

	using BufferAccessFlags = u32;

	class BufferMapping {
	public:
		friend class Buffer;

		BufferMapping();
		BufferMapping(const BufferMapping& mapping) = delete;
		BufferMapping& operator=(const BufferMapping& mapping) = delete;
		BufferMapping(BufferMapping&& mapping) = default;
		BufferMapping& operator=(BufferMapping&& mapping) = default;
		~BufferMapping();

		void unmap();
		bool is_valid() const;

		View<u8> view(u64 offset, u64 length) const;
		Slice<u8> slice(u64 offset, u64 length);

		inline View<u8> view() const { return view(0, _length); }
		inline Slice<u8> slice() { return slice(0, _length); }

		inline const Context* context() const { return _context; }

	private:
		BufferMapping(Context& context, GLuint buffer, u64 offset, u64 length, BufferAccessFlags access);

		Context* _context;
		GLuint _buffer;
		u8 *_data;
		u64 _length;
		BufferAccessFlags _access_flags;
	};

	class Buffer : public Object {
	public:
		static Buffer create(Context& context, BufferType type, u64 size, BufferFlags flags);
		static void copy(const Buffer& input, Buffer& output, u64 read_offset, u64 write_offset, u64 length);

		Buffer() = default;
		Buffer(const Buffer& buffer) = delete;
		Buffer& operator=(const Buffer& buffer) = delete;
		Buffer(Buffer&& buffer);
		Buffer& operator=(Buffer&& buffer);
		~Buffer();

		const BufferMapping map(u64 offset, u64 length) const;
		BufferMapping map(u64 offset, u64 length);
		inline BufferMapping map() const { return map(0, _size); }
		BufferMapping map() { return map(0, _size); }

		void destroy() override;
		bool is_valid() const override;

		inline GLuint id() const { return _id; }
		inline u64 size() const { return _size; }

		inline const Context& context() const override { return *_context; }
		inline Context& context() override { return *_context; }

	private:
		Context* _context;
		GLuint _id;
		u64 _size;
	};

}

#endif // AR_GRAPHICS_BUFFER_HH_
