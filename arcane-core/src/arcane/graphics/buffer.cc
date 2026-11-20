#include "./buffer.hh"

#include <arcane/core/log.hh>

namespace arcane::graphics {

	BufferMapping::BufferMapping(Context& context, GLuint buffer, u64 offset, u64 length, BufferAccessFlags access)
		: _context(&context), _buffer(buffer), _length(length), _access_flags(access) {
		AR_TRACE();
		_context->bind();

		GLbitfield opengl_access = 0;
		if (access & AR_BUFFER_ACCESS_FLAG_READ_BIT) opengl_access |= GL_MAP_READ_BIT;
		if (access & AR_BUFFER_ACCESS_FLAG_WRITE_BIT) opengl_access |= GL_MAP_WRITE_BIT;

		_data = reinterpret_cast<u8*>(glMapNamedBufferRange(buffer, offset, length, opengl_access));
	}

	BufferMapping::~BufferMapping() {
		unmap();
	}
	
	void BufferMapping::unmap() {
		if (_data == nullptr) return;
		AR_TRACE();
		_context->bind();
		glUnmapNamedBuffer(_buffer);
		_data = nullptr;
	}

	bool BufferMapping::is_valid() const {
		return _data != nullptr;
	}

	View<u8> BufferMapping::view(u64 offset, u64 length) const {
		if (!is_valid()) return View<u8>{};
		AR_TRACE();
		return View<u8>{ _data + offset, length };
	}

	Slice<u8> BufferMapping::slice(u64 offset, u64 length) {
		if (!is_valid()) return Slice<u8>{};
		AR_TRACE();
		return Slice<u8>{ _data + offset, length };
	}

	Buffer Buffer::create(Context &context, BufferType type, u64 size, BufferFlags flags) {
		if (!context.is_valid()) return Buffer{};
		AR_TRACE();

		GLbitfield opengl_buffer_flags = 0;
		if (flags & AR_BUFFER_FLAG_DYNAMIC_STORAGE) opengl_buffer_flags |= GL_DYNAMIC_STORAGE_BIT;
		if (flags & AR_BUFFER_FLAG_MAP_READABLE) opengl_buffer_flags |= GL_MAP_READ_BIT;
		if (flags & AR_BUFFER_FLAG_MAP_WRITEABLE) opengl_buffer_flags |= GL_MAP_WRITE_BIT;

		Buffer buffer{};
		buffer._context = &context;
		buffer._context->bind();
		buffer._size = size;

		glCreateBuffers(1, &buffer._id);
		glNamedBufferStorage(buffer._id, size, nullptr, opengl_buffer_flags);

		return buffer;
	}

	void Buffer::copy(const Buffer& input, Buffer& output, u64 read_offset, u64 write_offset, u64 length) {
		AR_ASSERT(input.is_valid(), "The input buffer is invalid.");
		AR_ASSERT(output.is_valid(), "The output buffer is invalid.");
		AR_ASSERT(read_offset + length < input.size(), "Offset + length is out of bounds for the input buffer.");
		AR_ASSERT(write_offset + length < output.size(), "Offset + length is out of bounds for the output buffer.");
		AR_ASSERT(input.context() == output.context(), "Input and output buffer must be from the same context.");
		AR_TRACE();
		output.context().bind();

		glCopyNamedBufferSubData(input.id(), output.id(), read_offset, write_offset, length);
	}

	Buffer::Buffer(Buffer&& buffer) : _context(buffer._context), _id(buffer._id), _size(buffer._size) {
		AR_TRACE();
		buffer._context = nullptr;
		buffer._id = 0;
		buffer._size = 0;
	}

	Buffer& Buffer::operator=(Buffer&& buffer) {
		AR_TRACE();

		_context = buffer._context;
		_id = buffer._id;
		_size = buffer._size;

		buffer._context = nullptr;
		buffer._id = 0;
		buffer._size = 0;

		return *this;
	}

	Buffer::~Buffer() {
		destroy();
	}

	void Buffer::destroy() {
		if (!is_valid()) return;
		AR_TRACE();
		_context->bind();

		glDeleteBuffers(1, &_id);

		_context = nullptr;
		_id = 0;
		_size = 0;
	}

	const BufferMapping Buffer::map(u64 offset, u64 length) const {
		AR_TRACE();
		return BufferMapping{ *_context, _id, offset, length, AR_BUFFER_ACCESS_FLAG_READ_BIT };
	}

	BufferMapping Buffer::map(u64 offset, u64 length) {
		AR_TRACE();
		return BufferMapping{ *_context, _id, offset, length, AR_BUFFER_ACCESS_FLAG_READ_BIT | AR_BUFFER_ACCESS_FLAG_WRITE_BIT };
	}
	
	bool Buffer::is_valid() const {
		if (_context == nullptr) return false;
		_context->bind();
		return glIsBuffer(_id);
	}

}
