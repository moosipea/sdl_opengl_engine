#version 130
precision highp float;

in vec3 ex_Color;
out vec4 ex_FragColor;

//layout(location = 2) uniform vec2 resolution;

void main(void) {
	//ex_FragColor = vec4(ex_Color, 1.0f);
	
	// assume width to be 640
	
//	vec2 uv = gl_FragCoord / resolution;
	
	vec2 uv = vec2(gl_FragCoord.x / 640.0f, gl_FragCoord.y / 480.0f);
	
	ex_FragColor = vec4(uv.x*sin(uv.x*48.0f), 0.0f, uv.y*sin(uv.y*48.0f), 1.0f);
}
