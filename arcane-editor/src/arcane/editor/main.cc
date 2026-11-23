#include "arcane/entity/camera.hh"
#include "arcane/graphics/pipeline.hh"
#include "arcane/system/memory.hh"
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
#include <arcane/entity/camera.hh>
#include <arcane/entity/transform.hh>
#include <cstdio>
#include <string_view>
#include <minilog.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

#define EDITOR_LOGGER 1

struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec4 color;
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

		arcane::HeapAllocator heap_allocator = {};

		arcane::SharedPtr<arcane::Window> window = arcane::Window::create(heap_allocator, "Arcane Engine", 1920 / 2, 1080 / 2);
		window.ptr()->show();

		arcane::SharedPtr<arcane::graphics::Context> context = arcane::graphics::Context::create(heap_allocator, window);

		Vertex vertices[] = {
			Vertex{ { -0.5f,  0.5f, 0.0f }, { -0.1f, 1.1f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			Vertex{ {  0.5f,  0.5f, 0.0f }, { 1.1f, 1.1f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			Vertex{ {  0.5f, -0.5f, 0.0f }, { 1.1f, -0.1f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
			Vertex{ { -0.5f, -0.5f, 0.0f }, { -0.1f, -0.1f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
		};

		struct ModelData {
			glm::mat4 model;
		};

		struct CameraData {
			glm::mat4 projection_matrix;
			glm::mat4 view_matrix;
		};

		ModelData model_data;
		CameraData camera_data;

		arcane::PerspectiveCamera camera = { { 0.0f, 0.0f, 1.0f } };
		camera.direction = { 0.0f, 0.0f, -1.0f };
		camera.field_of_view = glm::radians(90.0f);
		camera.aspect_ratio = static_cast<arcane::f32>(window.ptr()->width()) / static_cast<arcane::f32>(window.ptr()->height());
		camera.near_plane = 0.00001f;
		camera.far_plane = 1000.0f;

		camera_data.projection_matrix = camera.projection();
		camera_data.view_matrix = camera.view();

		arcane::Transform3D transform = { { 0.0f, 0.0f, -1.0f } };
		model_data.model = transform.model(); 

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
		
		arcane::SharedPtr<arcane::graphics::Texture> texture = arcane::graphics::Texture::create(heap_allocator, context, texture_info);
		texture.ptr()->set_image(0, 2, 2, arcane::graphics::ImageFormat::RGBA_U8, arcane::View<arcane::u8>{ reinterpret_cast<arcane::u8*>(texture_data), sizeof(texture_data) });

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
			.border_color = { 1.0f, 1.0f, 1.0f, 1.0f }
		};

		arcane::SharedPtr<arcane::graphics::Sampler> sampler = arcane::graphics::Sampler::create(heap_allocator, context, sampler_info);

		arcane::SharedPtr<arcane::graphics::Buffer> vertex_buffer = arcane::graphics::Buffer::create(
				heap_allocator,
				context,
				arcane::graphics::Buffer::Type::VERTEX,
				sizeof(vertices),
				arcane::graphics::AR_BUFFER_FLAG_MAP_READABLE | arcane::graphics::AR_BUFFER_FLAG_MAP_WRITEABLE);

		arcane::copy<Vertex>(
				arcane::View<Vertex>(vertices, sizeof(vertices) / sizeof(Vertex)),
				vertex_buffer.ptr()->map().as<Vertex>());

		vertex_buffer.ptr()->unmap();

		arcane::SharedPtr<arcane::graphics::Buffer> index_buffer = arcane::graphics::Buffer::create(
				heap_allocator,
				context,
				arcane::graphics::Buffer::Type::INDEX,
				sizeof(indices),
				arcane::graphics::AR_BUFFER_FLAG_MAP_READABLE | arcane::graphics::AR_BUFFER_FLAG_MAP_WRITEABLE);

		arcane::copy<arcane::u32>(
				arcane::View<arcane::u32>(indices, sizeof(indices) / sizeof(arcane::u32)),
				index_buffer.ptr()->map().as<arcane::u32>());

		index_buffer.ptr()->unmap();

		arcane::SharedPtr<arcane::graphics::Buffer> camera_data_buffer = arcane::graphics::Buffer::create(
				heap_allocator,
				context,
				arcane::graphics::Buffer::Type::UNIFORM,
				sizeof(camera_data),
				arcane::graphics::AR_BUFFER_FLAG_MAP_READABLE | arcane::graphics::AR_BUFFER_FLAG_MAP_WRITEABLE);

		arcane::copy<CameraData>(
				arcane::View<CameraData>(&camera_data, 1),
				camera_data_buffer.ptr()->map().as<CameraData>());

		camera_data_buffer.ptr()->unmap();

		arcane::SharedPtr<arcane::graphics::Buffer> model_data_buffer = arcane::graphics::Buffer::create(
				heap_allocator,
				context,
				arcane::graphics::Buffer::Type::UNIFORM,
				sizeof(model_data),
				arcane::graphics::AR_BUFFER_FLAG_MAP_READABLE | arcane::graphics::AR_BUFFER_FLAG_MAP_WRITEABLE);

		arcane::copy<ModelData>(
				arcane::View<ModelData>(&model_data, 1),
				model_data_buffer.ptr()->map().as<ModelData>());

		model_data_buffer.ptr()->unmap();

		std::vector<arcane::graphics::InputElement> input_layout = {
			{ 0, 0, 3, arcane::graphics::InputElement::Type::F32, offsetof(Vertex, position) },
			{ 1, 0, 2, arcane::graphics::InputElement::Type::F32, offsetof(Vertex, uv) },
			{ 2, 0, 4, arcane::graphics::InputElement::Type::F32, offsetof(Vertex, color) }
		};

		arcane::SharedPtr<arcane::graphics::Mesh> mesh = arcane::graphics::Mesh::create(heap_allocator, context);
		mesh.ptr()->set_vertex_buffer(0, vertex_buffer, 0, sizeof(Vertex));
		mesh.ptr()->set_index_buffer(index_buffer);
		mesh.ptr()->set_layout(input_layout);

		arcane::UniquePtr<arcane::graphics::Shader> vertex_shader = arcane::graphics::Shader::create(
					heap_allocator,
					context,
					arcane::graphics::Shader::Type::VERTEX,
					read_file_binary("./arcane-editor/bin/shaders/default/vertex.spv.o").view());

		arcane::UniquePtr<arcane::graphics::Shader> fragment_shader = arcane::graphics::Shader::create(
					heap_allocator,
					context,
					arcane::graphics::Shader::Type::PIXEL,
					read_file_binary("./arcane-editor/bin/shaders/default/fragment.spv.o").view());

		arcane::graphics::Pipeline::Info pipeline_info = {
			.primitive_type = arcane::graphics::Pipeline::PrimitiveType::TRIANGLE_LIST,
			.winding_order = arcane::graphics::Pipeline::WindingOrder::CLOCKWISE,
			.face_culling_mode = arcane::graphics::Pipeline::FaceCullingMode::BACK,
			.viewport = window.ptr()->inner_rect(),
			.scissor = window.ptr()->inner_rect(),
			.vertex_shader = vertex_shader.ptr(),
			.fragment_shader = fragment_shader.ptr() 
		};

		arcane::SharedPtr<arcane::graphics::Pipeline> pipeline = arcane::graphics::Pipeline::create(
				heap_allocator,
				context,
				pipeline_info);

		arcane::SharedPtr<arcane::graphics::Renderer> renderer = arcane::graphics::Renderer::create(
				heap_allocator, context);

		glfwSetInputMode(window.ptr()->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		arcane::f32 last_time = 0;
		arcane::f64 last_cursor_x, last_cursor_y;
		arcane::f64 pitch = 0.0, yaw = 0.0;
		while (!window.ptr()->should_close()) {
			arcane::f32 current_time = glfwGetTime();
			arcane::f32 delta_time = current_time - last_time;

			constexpr arcane::f32 CAMERA_SPEED = 1.0f;
			constexpr arcane::f32 CAMERA_SENSITIVITY = 0.01f;

			if (glfwGetKey(window.ptr()->handle(), GLFW_KEY_W)) camera.position += camera.direction * CAMERA_SPEED * delta_time;
			if (glfwGetKey(window.ptr()->handle(), GLFW_KEY_S)) camera.position -= camera.direction * CAMERA_SPEED * delta_time;
			if (glfwGetKey(window.ptr()->handle(), GLFW_KEY_D)) camera.position += glm::cross(camera.direction, glm::vec3(0.0f, 1.0f, 0.0f)) * CAMERA_SPEED * delta_time;
			if (glfwGetKey(window.ptr()->handle(), GLFW_KEY_A)) camera.position -= glm::cross(camera.direction, glm::vec3(0.0f, 1.0f, 0.0f)) * CAMERA_SPEED * delta_time;

			arcane::f64 cursor_x, cursor_y;
			glfwGetCursorPos(window.ptr()->handle(), &cursor_x, &cursor_y);
			const arcane::f64 cursor_delta_x = cursor_x - last_cursor_x;
			const arcane::f64 cursor_delta_y = last_cursor_y - cursor_y;
			last_cursor_x = cursor_x;
			last_cursor_y = cursor_y;

			pitch += CAMERA_SENSITIVITY * cursor_delta_y;
			pitch = glm::clamp(pitch, -89.99, 89.99);
			yaw -= CAMERA_SENSITIVITY * cursor_delta_x;

			camera.direction.x = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)); 
			camera.direction.y = glm::sin(glm::radians(pitch));
			camera.direction.z = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

			camera_data.projection_matrix = camera.projection();
			camera_data.view_matrix = camera.view();

			arcane::copy<CameraData>(
					arcane::View<CameraData>(&camera_data, 1),
					camera_data_buffer.ptr()->map().as<CameraData>());

			camera_data_buffer.ptr()->unmap();

			renderer.ptr()->begin();
			renderer.ptr()->clear();
			renderer.ptr()->set_mesh(mesh);
			renderer.ptr()->set_pipeline(pipeline);
			renderer.ptr()->set_viewport(window.ptr()->inner_rect());
			renderer.ptr()->set_scissor(window.ptr()->inner_rect());
			renderer.ptr()->set_texture_sampler(0, texture, sampler);
			renderer.ptr()->set_uniform_buffer(1, camera_data_buffer);
			renderer.ptr()->set_uniform_buffer(2, model_data_buffer);
			renderer.ptr()->draw_indexed(1, 6);
			renderer.ptr()->end();

			context.ptr()->swap_buffers();
			context.ptr()->unbind();
			window.ptr()->update();

			last_time = current_time;
		}

		window.ptr()->destroy();
	}

	ml_destroy_logger(EDITOR_LOGGER);
	ml_destroy_logger(AR_CORE_LOGGER_ID);
	return 0;
}
