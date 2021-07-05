#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "ShaderProgram.h"

struct FullscreenQuad
{
	FullscreenQuad();
	~FullscreenQuad();

	void Draw();
private:

	GLuint vao; 
	GLuint vbo;
	GLuint vertexPosLocation;

};

void lightLoad(ShaderProgram program, float angle);

void lightLoadParallax(ShaderProgram program, float angle);

#endif