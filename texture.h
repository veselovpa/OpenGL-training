#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "ShaderProgram.h"

struct Texture2D
{
	Texture2D(GLenum format, GLsizei width, GLsizei height, const void* data);
	explicit Texture2D(const std::string& file_name);

	~Texture2D();

	GLuint GetColorTexId() const { return tex; }
protected:

	GLuint tex;
};

struct RenderTexture2D
{
	RenderTexture2D(GLenum format, GLenum internal_format, GLsizei width, GLsizei height);

	~RenderTexture2D();

	void StartRender();
	void StopRender();

	GLuint GetColorTexId() const { return colorTex; }
	GLuint GetDepthTexId() const { return depthTex; }

private:

	GLuint CreateEmptyTex(GLint internal_format, GLenum format, GLsizei width, GLsizei height);
	GLuint colorTex;
	GLuint depthTex;
	GLuint fbo;

};

void bindTexture(const ShaderProgram& program, int unit, const std::string& name, Texture2D& texture);

void bindTexture(const ShaderProgram& program, int unit, const std::string& name, RenderTexture2D& texture);

#endif