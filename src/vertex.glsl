#version 130
precision highp float;

in vec4 in_Position;
in vec3 in_Color;

out vec3 ex_Color;

void main(void) {
	gl_Position = in_Position;
	ex_Color = in_Color;
}
