#include <glad/glad.h>
#include <arcane/core/platform.hh>
#include <arcane/core/types.hh>
#include <arcane/core/version.hh>
#include <arcane/core/log.hh>
#include <arcane/system/window.hh>
#include <arcane/graphics/context.hh>
#include <arcane/graphics/buffer.hh>
#include <arcane/graphics/mesh.hh>
#include <arcane/graphics/renderer.hh>
#include <arcane/data/buffer.hh>
#include <cstdio>
#include <string_view>
#include <minilog.h>
#include <minivector.h>
#include <GLFW/glfw3.h>

#define EDITOR_LOGGER 1

struct Vertex {
	mm_vec2_t position;
	mm_vec2_t uv;
	mm_vec4_t color;
};

arcane::Buffer<arcane::u8> read_file_binary(const std::string_view filename) {
	FILE* file = fopen(filename.data(), "rb");
	if (file == nullptr) return arcane::Buffer<arcane::u8>{};

	const long begin = ftell(file); 
	fseek(file, 0, SEEK_END);
	const long end = ftell(file); 
	fseek(file, 0, SEEK_SET);

	const size_t length = end - begin;

	arcane::Buffer<arcane::u8> buffer = arcane::Buffer<arcane::u8>::allocate(length);
	size_t bytes_read = fread(buffer.data(), sizeof(arcane::u8), length, file);
	if (bytes_read == 0) return arcane::Buffer<arcane::u8>{};

	fclose(file);

	return buffer;
}

int main(int argc, char** argv) {
	ml_create_logger("ARCANE", (ML_SEVERITY_FLAGS_ALL) ^ ML_LOG_SEVERITY_TRACE, stdout, AR_CORE_LOGGER_ID);
	ml_create_logger("EDITOR", ML_SEVERITY_FLAGS_ALL, stdout, EDITOR_LOGGER);

	{
		if (!arcane::Window::init()) {
			ml_log(EDITOR_LOGGER, ML_LOG_SEVERITY_FATAL, "Failed to initialize window manager.");
			return 1;
		}

		arcane::Window window = arcane::Window::create("Arcane Engine", 1920 / 2, 1080 / 2);
		window.show();

		arcane::graphics::Context context = arcane::graphics::Context::create(window);

		Vertex vertices[] = {
			Vertex{ { -0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			Vertex{ {  0.5f,  0.5f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			Vertex{ {  0.5f, -0.5f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			Vertex{ { -0.5f, -0.5f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
		};

		arcane::u32 indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		arcane::u32 texture_data[] = {
			0xffff0000, 0xff00ffff,
			0xff0000ff, 0xff00ff00
		};

		arcane::graphics::Texture::Info texture_info = {
			.type = arcane::graphics::Texture::Type::TEXTURE_2D,
			.format = arcane::graphics::ImageFormat::RGBA_U8,
			.width = 2,
			.height = 2,
			.depth = 1,
			.sample_count = 1,
			.level_count = 1
		};
		
		arcane::graphics::Texture texture = arcane::graphics::Texture::create(context, texture_info);
		texture.set_image(0, 2, 2, arcane::graphics::ImageFormat::RGBA_U8, arcane::View<arcane::u8>{ reinterpret_cast<arcane::u8*>(texture_data), sizeof(texture_data) });

		arcane::graphics::Sampler::Info sampler_info = {
			.min_filter = arcane::graphics::Sampler::SamplingMode::NEAREST,
			.mipmap_filter = arcane::graphics::Sampler::SamplingMode::NEAREST,
			.mag_filter = arcane::graphics::Sampler::SamplingMode::NEAREST,
			.lod_bias = 0.0f,
			.min_lod = -1000.0f,
			.max_lod = 1000.0f,
			.s_wrap = arcane::graphics::Sampler::WrapMode::CLAMP_TO_BORDER,
			.t_wrap = arcane::graphics::Sampler::WrapMode::CLAMP_TO_BORDER,
			.r_wrap = arcane::graphics::Sampler::WrapMode::CLAMP_TO_BORDER,
			.border_color = { 0.0f, 0.0f, 0.0f, 1.0f }
		};

		arcane::graphics::Sampler sampler = arcane::graphics::Sampler::create(context, sampler_info);

		arcane::graphics::Buffer vertex_buffer = arcane::graphics::Buffer::create(
				context,
				arcane::graphics::BufferType::VERTEX,
				sizeof(vertices),
				arcane::graphics::AR_BUFFER_FLAG_MAP_READABLE | arcane::graphics::AR_BUFFER_FLAG_MAP_WRITEABLE);

		arcane::copy<Vertex>(
				arcane::View<Vertex>(vertices, sizeof(vertices) / sizeof(Vertex)),
				vertex_buffer.map().slice().as<Vertex>());

		arcane::graphics::Buffer index_buffer = arcane::graphics::Buffer::create(
				context,
				arcane::graphics::BufferType::INDEX,
				sizeof(indices),
				arcane::graphics::AR_BUFFER_FLAG_MAP_READABLE | arcane::graphics::AR_BUFFER_FLAG_MAP_WRITEABLE);

		arcane::copy<arcane::u32>(
				arcane::View<arcane::u32>(indices, sizeof(indices) / sizeof(arcane::u32)),
				index_buffer.map().slice().as<arcane::u32>());

		std::vector<arcane::graphics::InputElement> input_layout = {
			{ 0, 0, 2, arcane::graphics::InputElement::Type::F32, offsetof(Vertex, position) },
			{ 1, 0, 2, arcane::graphics::InputElement::Type::F32, offsetof(Vertex, uv) },
			{ 2, 0, 4, arcane::graphics::InputElement::Type::F32, offsetof(Vertex, color) }
		};

		arcane::graphics::Mesh mesh = arcane::graphics::Mesh::create(context);
		mesh.set_vertex_buffer(0, vertex_buffer, 0, sizeof(Vertex));
		mesh.set_index_buffer(index_buffer);
		mesh.set_layout(input_layout);

		arcane::graphics::Pipeline::Info pipeline_info = {
			.primitive_type = arcane::graphics::Pipeline::PrimitiveType::TRIANGLE_LIST,
			.winding_order = arcane::graphics::Pipeline::WindingOrder::CLOCKWISE,
			.face_culling_mode = arcane::graphics::Pipeline::FaceCullingMode::BACK,
			.viewport = window.inner_rect(),
			.scissor = window.inner_rect(),
			.vertex_shader = arcane::graphics::Shader::create(
					context,
					arcane::graphics::Shader::Type::VERTEX,
					read_file_binary("./arcane-editor/bin/shaders/default/vertex.spv.o").view()),
			.fragment_shader = arcane::graphics::Shader::create(
					context,
					arcane::graphics::Shader::Type::PIXEL,
					read_file_binary("./arcane-editor/bin/shaders/default/fragment.spv.o").view())
		};

		arcane::graphics::Pipeline pipeline = arcane::graphics::Pipeline::create(context, pipeline_info);

		arcane::graphics::Renderer renderer = arcane::graphics::Renderer::create(context);

		while (!window.should_close()) {
			context.bind();

			renderer.begin();
			renderer.clear();
			renderer.set_mesh(mesh);
			renderer.set_pipeline(pipeline);
			renderer.set_viewport(window.inner_rect());
			renderer.set_scissor(window.inner_rect());
			renderer.set_texture_sampler(0, texture, sampler);
			renderer.draw_indexed(1, 6);
			renderer.end();

			context.swap_buffers();
			context.unbind();
			window.update();
		}

		window.destroy();
	}

	ml_destroy_logger(EDITOR_LOGGER);
	ml_destroy_logger(AR_CORE_LOGGER_ID);
	return 0;
}
