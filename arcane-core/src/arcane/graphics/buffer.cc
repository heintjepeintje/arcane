#include "./buffer.hh"

#include <arcane/core/log.hh>

namespace arcane::graphics {

	UniquePtr<Buffer> Buffer::create(IAllocator& allocator, const SharedPtr<Context> &context, Type type, u64 size, BufferFlags flags) {
		if (!context.is_valid()) return UniquePtr<Buffer>{};
		AR_TRACE();

		GLbitfield opengl_buffer_flags = 0;
		if (flags & AR_BUFFER_FLAG_DYNAMIC_STORAGE) opengl_buffer_flags |= GL_DYNAMIC_STORAGE_BIT;
		if (flags & AR_BUFFER_FLAG_MAP_READABLE) opengl_buffer_flags |= GL_MAP_READ_BIT;
		if (flags & AR_BUFFER_FLAG_MAP_WRITEABLE) opengl_buffer_flags |= GL_MAP_WRITE_BIT;

		UniquePtr<Buffer> buffer = UniquePtr<Buffer>::create(allocator);
		buffer.ptr()->_context = context.share();
		buffer.ptr()->_context.ptr()->bind();
		buffer.ptr()->_size = size;

		glCreateBuffers(1, &buffer.ptr()->_id);
		glNamedBufferStorage(buffer.ptr()->_id, size, nullptr, opengl_buffer_flags);

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

	Buffer::~Buffer() {
		destroy();
	}

	void Buffer::destroy() {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();

		glDeleteBuffers(1, &_id);

		_context.release();

		_id = 0;
		_size = 0;
	}

	View<u8> Buffer::map(u64 offset, u64 length) const {
		if (!is_valid()) return View<u8>{};
		AR_TRACE();
		_context.ptr()->bind();

		return View<u8> {
			reinterpret_cast<const u8*>(glMapNamedBufferRange(_id, offset, length, GL_MAP_READ_BIT)),
			length
		};
	}

	Slice<u8> Buffer::map(u64 offset, u64 length) {
		if (!is_valid()) return Slice<u8>{};
		AR_TRACE();
		_context.ptr()->bind();

		return Slice<u8> {
			reinterpret_cast<u8*>(glMapNamedBufferRange(_id, offset, length, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)),
			length
		};
	}

	void Buffer::unmap() {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ref().bind();

		glUnmapNamedBuffer(_id);
	}
	
	bool Buffer::is_valid() const {
		if (!_context.is_valid()) return false;
		if (!_context.ref().is_valid()) return false;
		_context.ref().bind();

		return glIsBuffer(_id);
	}

}
