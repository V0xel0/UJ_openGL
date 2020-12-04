#version 410

in vec2 vertex_uv;

uniform sampler2D diffuse_map; 

layout(std140) uniform Modifiers {
	float light_intenstiy;
	vec3 light_color;
};

layout(location=0) out vec4 vFragColor;

void main() {
	vec4 texColor = texture(diffuse_map, vertex_uv);
	vFragColor.rgb = texColor.rgb;
	vFragColor.a = 1.0f;
}