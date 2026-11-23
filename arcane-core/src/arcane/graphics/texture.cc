#include "./texture.hh"
#include "arcane/graphics/core.hh"
#include "arcane/system/memory.hh"

#include <arcane/core/log.hh>

namespace arcane::graphics {

	GLenum to_opengl_texture_type(Texture::Type type, u32 sample_count) {
		switch (type) {
			case Texture::Type::TEXTURE_1D: return GL_TEXTURE_1D;
			case Texture::Type::TEXTURE_1D_ARRAY: return GL_TEXTURE_1D_ARRAY;
			case Texture::Type::TEXTURE_2D: {
				if (sample_count > 1) {
					return GL_TEXTURE_2D_MULTISAMPLE;
				} else {
					return GL_TEXTURE_2D;
				}
			}
			case Texture::Type::TEXTURE_2D_ARRAY: {
				if (sample_count > 1) {
					return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
				} else {
					return GL_TEXTURE_2D_ARRAY;
				}
			}
			case Texture::Type::TEXTURE_3D: return GL_TEXTURE_3D;
			default: return GL_NONE;
		}
	}

	GLenum to_opengl_internal_format(ImageFormat format) {
		switch (format) {
			case ImageFormat::R_I8: return GL_R8I;
			case ImageFormat::R_I16: return GL_R16I;
			case ImageFormat::R_I32: return GL_R32I;
			case ImageFormat::RG_I8: return GL_RG8I;
			case ImageFormat::RG_I16: return GL_RG16I;
			case ImageFormat::RG_I32: return GL_RG32I;
			case ImageFormat::RGB_I8: return GL_RGB8I;
			case ImageFormat::RGB_I16: return GL_RGB16I;
			case ImageFormat::RGB_I32: return GL_RGB32I;
			case ImageFormat::RGBA_I8: return GL_RGBA8I;
			case ImageFormat::RGBA_I16: return GL_RGBA16I;
			case ImageFormat::RGBA_I32: return GL_RGBA32I;

			case ImageFormat::R_U8: return GL_R8;
			case ImageFormat::R_U16: return GL_R16;
			case ImageFormat::R_U32: return GL_R32UI;
			case ImageFormat::RG_U8: return GL_RG8;
			case ImageFormat::RG_U16: return GL_RG16;
			case ImageFormat::RG_U32: return GL_RG32UI;
			case ImageFormat::RGB_U8: return GL_RGB8;
			case ImageFormat::RGB_U16: return GL_RGB16;
			case ImageFormat::RGB_U32: return GL_RGB32UI;
			case ImageFormat::RGBA_U8: return GL_RGBA8;
			case ImageFormat::RGBA_U16: return GL_RGBA16;
			case ImageFormat::RGBA_U32: return GL_RGBA32UI;

			default: return GL_NONE;
		}
	}

	GLenum to_opengl_pixel_format(ImageFormat format) {
		switch (format) {
			case ImageFormat::R_I8: return GL_RED;
			case ImageFormat::R_I16: return GL_RED;
			case ImageFormat::R_I32: return GL_RED;
			case ImageFormat::RG_I8: return GL_RG;
			case ImageFormat::RG_I16: return GL_RG;
			case ImageFormat::RG_I32: return GL_RG;
			case ImageFormat::RGB_I8: return GL_RGB;
			case ImageFormat::RGB_I16: return GL_RGB;
			case ImageFormat::RGB_I32: return GL_RGB;
			case ImageFormat::RGBA_I8: return GL_RGBA;
			case ImageFormat::RGBA_I16: return GL_RGBA;
			case ImageFormat::RGBA_I32: return GL_RGBA;

			case ImageFormat::R_U8: return GL_RED;
			case ImageFormat::R_U16: return GL_RED;
			case ImageFormat::R_U32: return GL_RED;
			case ImageFormat::RG_U8: return GL_RG;
			case ImageFormat::RG_U16: return GL_RG;
			case ImageFormat::RG_U32: return GL_RG;
			case ImageFormat::RGB_U8: return GL_RGB;
			case ImageFormat::RGB_U16: return GL_RGB;
			case ImageFormat::RGB_U32: return GL_RGB;
			case ImageFormat::RGBA_U8: return GL_RGBA;
			case ImageFormat::RGBA_U16: return GL_RGBA;
			case ImageFormat::RGBA_U32: return GL_RGBA;

			default: return GL_NONE;
		}
	}

	void allocate_texture_storage(Texture::Type type, GLuint id, u32 sample_count, ImageFormat format, u32 level_count, u32 width, u32 height, u32 depth) {
		AR_TRACE();
		GLenum opengl_internal_format = to_opengl_internal_format(format);

		if (type == Texture::Type::TEXTURE_1D) {
			glTextureStorage1D(id, level_count, opengl_internal_format, width);		
		} else if (type == Texture::Type::TEXTURE_1D_ARRAY) {
			glTextureStorage2D(id, level_count, opengl_internal_format, width, height);		
		} else if (type == Texture::Type::TEXTURE_2D && sample_count > 1) {
			glTextureStorage2DMultisample(id, sample_count, opengl_internal_format, width, height, GL_FALSE);
		} else if (type == Texture::Type::TEXTURE_2D) {
			glTextureStorage2D(id, level_count, opengl_internal_format, width, height);
		} else if (type == Texture::Type::TEXTURE_2D_ARRAY && sample_count > 1) {
			glTextureStorage3DMultisample(id, sample_count, opengl_internal_format, width, height, depth, GL_FALSE);
		} else if (type == Texture::Type::TEXTURE_2D_ARRAY) {
			glTextureStorage3D(id, level_count, opengl_internal_format, width, height, depth);
		}
	}

	UniquePtr<Texture> Texture::create(IAllocator& allocator, const SharedPtr<Context>& context, const Info& info) {
		AR_TRACE();
		UniquePtr<Texture> texture = UniquePtr<Texture>::create(allocator);
		texture.ptr()->_context = context.share();
		texture.ptr()->_context.ptr()->bind();

		glCreateTextures(to_opengl_texture_type(info.type, info.sample_count), 1, &texture.ptr()->_id);
		allocate_texture_storage(info.type, texture.ptr()->_id, info.sample_count, info.format, info.level_count, info.width, info.height, info.depth);

		return texture;
	}

	Texture::~Texture() {
		destroy();
	}

	void Texture::set_image(u32 level, u32 width, u32 height, ImageFormat format, const View<u8>& data) {
		if (!is_valid()) return;
		AR_TRACE();
		_context.ptr()->bind();
		glTextureSubImage2D(_id, level, 0, 0, width, height, to_opengl_pixel_format(format), GL_UNSIGNED_BYTE, data.data());
	}

	void Texture::destroy() {
		if (!is_valid()) return;
		AR_TRACE();

		_context.ptr()->bind();

		glDeleteTextures(1, &_id);
		_id = 0;

		_context.release();
	}

	bool Texture::is_valid() const {
		if (!_context.is_valid()) return false;
		if (!_context.ptr()->is_valid()) return false;

		_context.ptr()->bind();
		return glIsTexture(_id);
	}

	GLenum to_opengl_min_filter(Sampler::SamplingMode mode, Sampler::SamplingMode mipmap_mode) {
		if (mode == Sampler::SamplingMode::NEAREST && mipmap_mode == Sampler::SamplingMode::NEAREST) return GL_NEAREST_MIPMAP_NEAREST;
		if (mode == Sampler::SamplingMode::NEAREST && mipmap_mode == Sampler::SamplingMode::LINEAR) return GL_NEAREST_MIPMAP_LINEAR;
		if (mode == Sampler::SamplingMode::LINEAR && mipmap_mode == Sampler::SamplingMode::NEAREST) return GL_LINEAR_MIPMAP_NEAREST;
		if (mode == Sampler::SamplingMode::LINEAR && mipmap_mode == Sampler::SamplingMode::LINEAR) return GL_LINEAR_MIPMAP_LINEAR;
		return GL_NONE;
	}

	GLenum to_opengl_mag_filter(Sampler::SamplingMode mode) {
		switch (mode) {
			case Sampler::SamplingMode::NEAREST: return GL_NEAREST;
			case Sampler::SamplingMode::LINEAR: return GL_LINEAR;
			default: return GL_NONE;
		}
	}

	GLenum to_opengl_sampler_wrap(Sampler::WrapMode wrap) {
		switch (wrap) {
			case Sampler::WrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
			case Sampler::WrapMode::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
			case Sampler::WrapMode::MIRRORED_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
			case Sampler::WrapMode::REPEAT: return GL_REPEAT;
			case Sampler::WrapMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
			default: return GL_NONE;
		}
	}

	UniquePtr<Sampler> Sampler::create(IAllocator& allocator, const SharedPtr<Context>& context, const Info& info) {
		UniquePtr<Sampler> sampler = UniquePtr<Sampler>::create(allocator);
		sampler.ptr()->_context = context.share();
		sampler.ptr()->_context.ptr()->bind();

		glCreateSamplers(1, &sampler.ptr()->_id);
		glSamplerParameterf(sampler.ptr()->_id, GL_TEXTURE_LOD_BIAS, info.lod_bias);
		glSamplerParameteri(sampler.ptr()->_id, GL_TEXTURE_MIN_FILTER, to_opengl_min_filter(info.min_filter, info.mipmap_filter));
		glSamplerParameteri(sampler.ptr()->_id, GL_TEXTURE_MAG_FILTER, to_opengl_mag_filter(info.min_filter));
		glSamplerParameterf(sampler.ptr()->_id, GL_TEXTURE_MIN_LOD, info.min_lod);
		glSamplerParameterf(sampler.ptr()->_id, GL_TEXTURE_MAX_LOD, info.max_lod);
		glSamplerParameterf(sampler.ptr()->_id, GL_TEXTURE_WRAP_S, to_opengl_sampler_wrap(info.s_wrap));
		glSamplerParameterf(sampler.ptr()->_id, GL_TEXTURE_WRAP_T, to_opengl_sampler_wrap(info.t_wrap));
		glSamplerParameterf(sampler.ptr()->_id, GL_TEXTURE_WRAP_R, to_opengl_sampler_wrap(info.r_wrap));

		f32 border_color[] = { info.border_color.x, info.border_color.y, info.border_color.z, info.border_color.w };
		glSamplerParameterfv(sampler.ptr()->_id, GL_TEXTURE_BORDER_COLOR, border_color);

		return sampler;
	}

	Sampler::~Sampler() {
		destroy();
	}

	void Sampler::destroy() {
		if (!is_valid()) return;

		_context.ptr()->bind();

		glDeleteSamplers(1, &_id);
		_id = 0;

		_context.release();
	}

	bool Sampler::is_valid() const {
		if (!_context.is_valid()) return false;
		if (!_context.ptr()->is_valid()) return false;
		_context.ptr()->bind();
		return glIsSampler(_id);
	}

}
