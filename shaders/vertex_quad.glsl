#version 330 core
layout(location = 0) in vec2 vertex;

out vec2 fragmentTexCoord;
out vec2 vFragPosition;

void main()
{
	fragmentTexCoord = vertex * 0.5f + 0.5f;
	vFragPosition = vertex;

	gl_Position = vec4(vertex, 0.0f, 1.0f);
}