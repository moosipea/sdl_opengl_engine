#version 130
precision highp float;

in vec3 ex_Color;
out vec4 ex_FragColor;

uniform vec2 iResolution;
uniform vec2 iShift;
uniform float iZoom;

vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0f - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0f, 1.0f), c.y);
}

void main(void) {
	vec2 uv = vec2(gl_FragCoord.x / iResolution.x, gl_FragCoord.y / iResolution.y);
	
	vec2 mandelbrot_scale = vec2((uv.x * 2.47f - 2.0f) + iShift.x, (uv.y * 2.24f - 1.12) + iShift.y) / iZoom;
	
	float x = 0.0f;
	float y = 0.0f;

	int iter = 10;
	int max_iter = 100;
	while(x*x + y*y <= 4.0f && iter < max_iter) {
		float xtemp = x*x - y*y + mandelbrot_scale.x;
		y = 2.0f * x * y + mandelbrot_scale.y;
		x = xtemp;
		iter++;
	}
	
	float b = 1.0f - float(iter) / float(max_iter);
	float v = b < 0.2f ? smoothstep(0.5, 0.0f, (0.2 - b) * 2.5f) : 0.5f;
	vec3 col = hsv2rgb(vec3(b, 1.0f, v));	
	ex_FragColor = vec4(col, 1.0f);
}
