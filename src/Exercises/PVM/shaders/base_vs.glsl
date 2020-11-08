#version 410

layout(location=0) in vec3 a_vertex_position;
layout(location=1) in vec3 a_vertex_color;

layout(std140) uniform Matrices {
	mat4 Model;
	mat4 View;
	mat4 Proj;
	mat4 PVM;
};

out vec3 vertex_color;

void main() {
	gl_Position = PVM*vec4(a_vertex_position, 1.0f);
	vertex_color = a_vertex_color;
}