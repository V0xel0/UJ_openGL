#version 410

in vec3 vertex_color;

layout(std140) uniform Modifiers {
	float light_intenstiy;
	vec3 light_color;
};

layout(location=0) out vec4 vFragColor;


float drawCircle(vec2 p, float d, vec2 uv)
{
	return 1.0 - smoothstep(d-(d*0.1), d+(d*0.1),  dot(p-uv, p-uv)*1e4);
}

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

	// Palm -- SDF math
	#if 0
	vec2 p = gl_FragCoord.xy / resolution;
	vec2 q = p - vec2(0.33,0.7);
		
	vec3 col = mix( vec3(1.0,0.3,0.0), vec3(1.0,0.8,0.3), sqrt(p.y) );
	
	float r = 0.2 + 0.1*cos( atan(q.y,q.x)*10.0 + 20.0*q.x + 1.0);
	col *= smoothstep( r, r+0.01, length( q ) );

	r = 0.015;
	r += 0.002*sin(120.0*q.y);
	r += exp(-40.0*p.y);
	col *= 1.0 - (1.0-smoothstep(r,r+0.002, abs(q.x-0.25*sin(2.0*q.y))))*(1.0-smoothstep(0.0,0.1,q.y));
	
	vFragColor = vec4(col,1.0);
	#endif
}
