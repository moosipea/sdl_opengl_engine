#version 130
precision highp float;

in vec3 ex_Color;
out vec4 ex_FragColor;

//uniform vec2 iResolution;

void main(void) {
	vec2 uv = vec2(gl_FragCoord.x / 640.0f, gl_FragCoord.y / 480.0f);
	ex_FragColor = vec4(uv.x, 0.0f, uv.y, 1.0f);
}
