#version 410

in vec3 vertex_color;

layout(std140) uniform Modifiers {
	float light_intenstiy;
	vec3 light_color;
};

layout(location=0) out vec4 vFragColor;

void main() {
	vec2 resolution = vec2(650, 480);
	vec2 uv = gl_FragCoord.xy / resolution;
	#if 0
	vFragColor = vec4(uv * vec2(1.33), 0.5, 1.0);
	#endif

	#if 1
	vFragColor.a = 1.0f;
	vFragColor.rgb = light_intenstiy*light_color*vertex_color.rgb;
	#endif	
}