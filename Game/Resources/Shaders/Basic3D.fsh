#version 330 core

in float v_intensity;
in vec2 v_texture;
uniform vec4 u_mainTextColor;

uniform sampler2D u_texture;

out vec4 fragColor;

void main()
{
  fragColor = texture(u_texture, v_texture).rgba * (vec4(v_intensity, v_intensity, v_intensity, 1.0) * u_mainTextColor);
}
