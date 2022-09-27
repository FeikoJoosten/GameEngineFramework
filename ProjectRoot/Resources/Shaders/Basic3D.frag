#version 330 core

in float v_intensity;
in vec2 v_texture;
in vec3 normal;
uniform vec4 u_mainTextColor;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
	//fragColor = vec4(normal, 1.0);
	fragColor = texture(u_texture, v_texture) * (vec4(v_intensity, v_intensity, v_intensity, 1.0) * u_mainTextColor);
}