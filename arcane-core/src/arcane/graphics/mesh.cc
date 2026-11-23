#include "./mesh.hh"
#include "arcane/graphics/buffer.hh"

namespace arcane::graphics {
	
	GLenum to_opengl_input_element_type(InputElement::Type type) {
		switch (type) {
			case InputElement::Type::I8: return GL_BYTE;
			case InputElement::Type::I16: return GL_SHORT;
			case InputElement::Type::I32: return GL_INT;
			case InputElement::Type::U8: return GL_UNSIGNED_BYTE;
			case InputElement::Type::U16: return GL_UNSIGNED_SHORT;
			case InputElement::Type::U32: return GL_UNSIGNED_INT;
			case InputElement::Type::F32: return GL_FLOAT;
			default: return GL_NONE;
		}
	}

	UniquePtr<Mesh> Mesh::create(IAllocator& allocator, const SharedPtr<Context>& context) {
		AR_TRACE();

		UniquePtr<Mesh> mesh = UniquePtr<Mesh>::create(allocator);
		mesh.ptr()->_context = context.share();
		mesh.ptr()->_context.ptr()->bind();
		glCreateVertexArrays(1, &mesh.ptr()->_id);

		return mesh;
	}

	Mesh::~Mesh() {
		destroy();
	}

	void Mesh::destroy() {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();

		for (SharedPtr<Buffer>& vertex_buffer : _vertex_buffers) {
			vertex_buffer.release();
		}

		_index_buffer.release();

		glDeleteVertexArrays(1, &_id);
		_id = 0;

		_context.release();
	}

	bool Mesh::is_valid() const {
		if (!_context.is_valid()) return false;
		if (!_context.ptr()->is_valid()) return false;
		_context.ptr()->bind();
		return glIsVertexArray(_id);
	}

	void Mesh::set_vertex_buffer(u32 index, const SharedPtr<Buffer>& buffer, u64 offset, u64 stride) {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();
		glVertexArrayVertexBuffer(_id, index, buffer.ptr()->id(), offset, stride);

		if (_vertex_buffers.size() > index) _vertex_buffers[index].release();
		_vertex_buffers.emplace(_vertex_buffers.begin() + index, buffer.share());
	}

	void Mesh::set_index_buffer(const SharedPtr<Buffer>& buffer) {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();
		glVertexArrayElementBuffer(_id, buffer.ptr()->id());

		_index_buffer.release();
		_index_buffer = buffer.share();
	}

	void Mesh::set_layout(const std::vector<InputElement>& layout) {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();

		for (const InputElement& element : layout) {
			glEnableVertexArrayAttrib(_id, element.attribute_index);
			glVertexArrayAttribBinding(_id, element.attribute_index, element.buffer_index);
			glVertexArrayAttribFormat(_id, element.attribute_index, element.count, to_opengl_input_element_type(element.type), GL_FALSE, element.offset);
		}

		_layout = layout;
	}

}
