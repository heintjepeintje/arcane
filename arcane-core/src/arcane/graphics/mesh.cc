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

	Mesh Mesh::create(Context& context) {
		AR_TRACE();

		Mesh mesh;
		mesh._context = &context;
		mesh._context->bind();
		glCreateVertexArrays(1, &mesh._id);

		return mesh;
	}

	Mesh::Mesh() { }

	Mesh::~Mesh() {
		AR_TRACE();
		destroy();
	}

	void Mesh::destroy() {
		if (!is_valid()) return;
		AR_TRACE();
		_context->bind();
		glDeleteVertexArrays(1, &_id);
	}

	bool Mesh::is_valid() const {
		if (_context == nullptr) return false;
		_context->bind();
		return glIsVertexArray(_id);
	}

	void Mesh::set_vertex_buffer(u32 index, const Buffer& buffer, u64 offset, u64 stride) {
		AR_TRACE();
		_context->bind();
		glVertexArrayVertexBuffer(_id, index, buffer.id(), offset, stride);
	}

	void Mesh::set_index_buffer(const Buffer& buffer) {
		AR_TRACE();
		_context->bind();
		glVertexArrayElementBuffer(_id, buffer.id());
	}

	void Mesh::set_layout(const std::vector<InputElement>& layout) {
		AR_TRACE();
		_context->bind();
		for (const InputElement& element : layout) {
			glEnableVertexArrayAttrib(_id, element.attribute_index);
			glVertexArrayAttribBinding(_id, element.attribute_index, element.buffer_index);
			glVertexArrayAttribFormat(_id, element.attribute_index, element.count, to_opengl_input_element_type(element.type), GL_FALSE, element.offset);
		}
	}

}
