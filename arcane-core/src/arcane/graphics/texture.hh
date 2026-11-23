#ifndef AR_GRAPHICS_TEXTURE_HH_
#define AR_GRAPHICS_TEXTURE_HH_

#include <arcane/core/types.hh>
#include "./core.hh"
#include "./context.hh"
#include <arcane/data/view.hh>

namespace arcane::graphics {

	enum class ImageFormat {
		NONE,
		R_I8, R_I16, R_I32,
		RG_I8, RG_I16, RG_I32,
		RGB_I8, RGB_I16, RGB_I32,
		RGBA_I8, RGBA_I16, RGBA_I32,

		R_U8, R_U16, R_U32,
		RG_U8, RG_U16, RG_U32,
		RGB_U8, RGB_U16, RGB_U32,
		RGBA_U8, RGBA_U16, RGBA_U32
	};

	class Texture : public Object {
	public:
		enum class Type {
			NONE,
			TEXTURE_1D, TEXTURE_1D_ARRAY,
			TEXTURE_2D, TEXTURE_2D_ARRAY,
			TEXTURE_3D, CUBE_MAP
		};

		struct Info {
			Type type;
			ImageFormat format;
			u32 width, height, depth;
			u32 sample_count;
			u32 level_count;
			u32 layer_count;
		};

		static UniquePtr<Texture> create(IAllocator& allocator, const SharedPtr<Context>& context, const Info& info);

		~Texture();

		void destroy() override;
		bool is_valid() const override;

		void set_image(u32 level, u32 width, u32 height, ImageFormat format, const View<u8>& data);

		inline Context& context() override { return _context.ref(); }
		inline const Context& context() const override { return _context.ref(); }

		inline GLuint id() const { return _id; }

		inline Type type() const { return _type; }
		inline u32 sample_count() const { return _sample_count; }
		inline ImageFormat format() const { return _format; }
		inline u32 width() const { return _width; }
		inline u32 height() const { return _height; }
		inline u32 depth() const { return _depth; }
		inline u32 level_count() const { return _level_count; }

	private:
		SharedPtr<Context> _context;
		GLuint _id;
		Type _type;
		u32 _sample_count;
		ImageFormat _format;
		u32 _width, _height, _depth;
		u32 _level_count;
	};

	class Sampler : public Object {
	public:
		enum class SamplingMode {
			NONE = 0,
			NEAREST,
			LINEAR
		};

		enum class WrapMode {
			NONE = 0,
			CLAMP_TO_BORDER,
			CLAMP_TO_EDGE,
			MIRRORED_CLAMP_TO_EDGE,
			REPEAT,
			MIRRORED_REPEAT
		};

		struct Info {
			SamplingMode min_filter;
			SamplingMode mipmap_filter;
			SamplingMode mag_filter;
			float lod_bias;
			float min_lod, max_lod;
			WrapMode s_wrap, t_wrap, r_wrap;
			glm::vec4 border_color;
		};

		static UniquePtr<Sampler> create(IAllocator& allocator, const SharedPtr<Context>& context, const Info& info);

		~Sampler();

		void destroy() override;
		bool is_valid() const override;

		inline Context& context() override { return _context.ref(); }
		inline const Context& context() const override { return _context.ref(); }

		inline GLuint id() const { return _id; }

		inline SamplingMode min_filter() const { return _min_filter; }
		inline SamplingMode mipmap_filter() const { return _mipmap_filter; }
		inline SamplingMode mag_filter() const { return _mag_filter; }
		inline f32 lod_bias() const { return  _lod_bias; }
		inline f32 min_lod() const { return _min_lod; }
		inline f32 max_lod() const { return _max_lod; }

	private:
		SharedPtr<Context> _context;
		GLuint _id;
		SamplingMode _min_filter;
		SamplingMode _mipmap_filter;
		SamplingMode _mag_filter;
		f32 _lod_bias; 
		f32 _min_lod, _max_lod;
	};

}

#endif // AR_GRAPHICS_TEXTURE_HH_
