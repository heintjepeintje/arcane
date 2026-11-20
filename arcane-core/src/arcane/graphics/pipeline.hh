#ifndef AR_GRAPHICS_PIPELINE_HH_
#define AR_GRAPHICS_PIPELINE_HH_

#include <arcane/core/types.hh>
#include "./core.hh"
#include "./context.hh"
#include <arcane/geometry/shape2d.hh>
#include <arcane/data/view.hh>

namespace arcane::graphics {

	struct InputElement {
		enum class Type {
			I8, I16, I32,
			U8, U16, U32,
			F32
		};

		u32 attribute_index;
		u32 buffer_index;
		u32 count;
		Type type;
		u64 offset;
	};

	class Shader : public Object {
	public:
		enum class Type {
			VERTEX,
			PIXEL
		};

		static Shader create(Context& context, Type type, const View<u8>& binary);

		Shader();
		Shader(const Shader& copy) = delete;
		Shader& operator=(const Shader& copy) = delete;
		Shader(Shader&& move);
		Shader& operator=(Shader&& move);
		~Shader();

		inline GLuint id() const { return _id; }

		void destroy() override;
		bool is_valid() const override;

		inline Context& context() override { return *_context; }
		inline const Context& context() const override { return *_context; }
	private:
		Context* _context;
		Type _type;
		GLuint _id;
	};

	class Pipeline : public Object {
	public:
		enum class PrimitiveType {
			POINT_LIST,
			LINE_STRIP,
			LINE_LOOP,
			LINE_LIST,
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
			TRIANGLE_LIST
		};

		enum class WindingOrder {
			CLOCKWISE,
			COUNTER_CLOCKWISE
		};

		enum class FaceCullingMode {
			FRONT,
			BACK,
			FRONT_AND_BACK
		};

		struct Info {
			PrimitiveType primitive_type;
			WindingOrder winding_order;
			FaceCullingMode face_culling_mode;
			Rect2D viewport;
			Rect2D scissor;
			Shader vertex_shader;
			Shader fragment_shader;
		};

		static Pipeline create(Context& context, const Info& info);

		Pipeline();
		Pipeline(const Pipeline& copy) = delete;
		Pipeline& operator=(const Pipeline& copy) = delete;
		Pipeline(Pipeline&& move);
		Pipeline& operator=(Pipeline&& move);
		~Pipeline();

		void destroy() override;
		bool is_valid() const override;

		inline PrimitiveType primitive_type() const { return _data.primitive_type; }
		inline WindingOrder winding_order() const { return _data.winding_order; }
		inline FaceCullingMode face_culling_mode() const { return _data.face_culling_mode; }
		inline Rect2D viewport() const { return _data.viewport; }
		inline Rect2D scissor() const { return _data.scissor; }
		inline GLuint program_id() const { return _program_id; }

		inline Context& context() override { return *_context; }
		inline const Context& context() const override { return *_context; }

	private:
		struct Data {
			PrimitiveType primitive_type; 
			WindingOrder winding_order;
			FaceCullingMode face_culling_mode;
			Rect2D viewport, scissor;
		};

		Context* _context;
		Data _data;
		GLuint _program_id;
	};

}

#endif // AR_GRAPHICS_PIPELINE_HH_
