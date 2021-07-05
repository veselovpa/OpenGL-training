#include "Geometry.h"

FullscreenQuad::FullscreenQuad()
{
	float quadPos[] =
	{
		-1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f
	};

	vertexPosLocation = 0;

	glGenBuffers(1, &vbo); GL_CHECK_ERRORS;
	glBindBuffer(GL_ARRAY_BUFFER, vbo); GL_CHECK_ERRORS;

	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), (GLfloat*)quadPos, GL_STATIC_DRAW); GL_CHECK_ERRORS;

	glGenVertexArrays(1, &vao); GL_CHECK_ERRORS;
	glBindVertexArray(vao); GL_CHECK_ERRORS;

	glBindBuffer(GL_ARRAY_BUFFER, vbo); GL_CHECK_ERRORS;
	glEnableVertexAttribArray(vertexPosLocation); GL_CHECK_ERRORS;
	glVertexAttribPointer(vertexPosLocation, 2, GL_FLOAT, GL_FALSE, 0, 0); GL_CHECK_ERRORS;

	glBindVertexArray(0);

}

FullscreenQuad::~FullscreenQuad()
{
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0; 
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}

void FullscreenQuad::Draw()
{
	glBindVertexArray(vao); GL_CHECK_ERRORS;
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void lightLoad(ShaderProgram program, float angle)
{
	program.SetUniform("lightPos[0].position", make_float3(cosf(angle / 10.0f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f) * 46.51f - 25.0f));
	program.SetUniform("lightPos[0].color", make_float3(1.0f, 1.0f, 1.0f));

	program.SetUniform("lightPos[1].position", make_float3(cosf(angle / 10.0f + 3.14f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f + 3.14f) * 46.51f - 25.0f));
	program.SetUniform("lightPos[1].color", make_float3(1.0f, 1.0f, 1.0f));

	program.SetUniform("light[0].dir", make_float3(0.0f, -1.0f, 1.0f));
	program.SetUniform("light[0].color", make_float3(0.7f, 0.7f, 0.7f));

	program.SetUniform("light[1].dir", make_float3(1.0f, -1.0f, 0.0f));
	program.SetUniform("light[1].color", make_float3(0.8f, 0.8f, 0.8f));
}

void lightLoadParallax(ShaderProgram program, float angle)
{
	program.SetUniform("lightPos[0].position", make_float3(cosf(angle / 10.0f) * 46.51f + 25.0f, 20.5f, sinf(angle / 10.f) * 46.51f - 25.0f));
	program.SetUniform("lightPos[0].color", make_float3(1.0f, 1.0f, 1.0f));

	program.SetUniform("light[0].dir", make_float3(0.0f, -1.0f, 1.0f));
	program.SetUniform("light[0].color", make_float3(0.7f, 0.7f, 0.7f));

	program.SetUniform("light[1].dir", make_float3(1.0f, -1.0f, 0.0f));
	program.SetUniform("light[1].color", make_float3(0.8f, 0.8f, 0.8f));
}