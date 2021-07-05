#version 330 core

in vec2 fragmentTexCoord;
in vec2 vFragPosition;

out vec4 outColor;

uniform sampler2D colorTexture;

void main()
{
	vec3 color = textureLod(colorTexture, fragmentTexCoord, 0).xyz;
	vec3 mapped = vec3(1.0f, 1.0f, 1.0f);

	mapped = vec3(1.0f, 1.0f, 1.0f) - exp(-color * 0.9f);

	outColor = vec4(mapped,  1.0f);
}