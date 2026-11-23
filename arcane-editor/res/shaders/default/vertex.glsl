#version 430 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;

layout (location = 0) out vec2 out_uv;
layout (location = 1) out vec4 out_color;

layout (binding = 1, std140) uniform CameraData {
	mat4 projection;
	mat4 view;
} u_camera_data;

layout (binding = 2, std140) uniform ModelData {
	mat4 model;
} u_model_data;

void main() {
	out_uv = in_uv;
	out_color = in_color;
	gl_Position = u_camera_data.projection * u_camera_data.view * u_model_data.model * vec4(in_position, 1.0);
}
