#include "Texture.h"

#include "stb_image.h"


Texture2D::Texture2D(GLenum format, GLsizei width, GLsizei height, const void* data)
{
    glGenTextures(1, &tex); GL_CHECK_ERRORS;
    glBindTexture(GL_TEXTURE_2D, tex); GL_CHECK_ERRORS;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  GL_CHECK_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GL_CHECK_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  GL_CHECK_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  GL_CHECK_ERRORS;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); GL_CHECK_ERRORS;
    glGenerateMipmap(GL_TEXTURE_2D);
}


Texture2D::Texture2D(const std::string& filename)
{
    int w;
    int h;
    int nComponents;
    unsigned char* imageData = stbi_load(filename.c_str(), &w, &h, &nComponents, STBI_rgb);

    if (imageData)
    {
        GLenum format = GL_RGB;
        if (nComponents == 1)
            format = GL_RED;
        else if (nComponents == 3)
            format = GL_RGB;
        else if (nComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(imageData);
    }
    else
    {
        std::cout << "Failed to load texture: " << filename << std::endl;
        stbi_image_free(imageData);
    }

}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &tex);
    tex = -1;
}

void bindTexture(const ShaderProgram& program, int unit, const std::string& name, Texture2D& texture)
{
    glActiveTexture(GL_TEXTURE0 + unit);  GL_CHECK_ERRORS;
    glBindTexture(GL_TEXTURE_2D, texture.GetColorTexId());  GL_CHECK_ERRORS;

    program.SetUniform(name, unit); GL_CHECK_ERRORS;
}

RenderTexture2D::RenderTexture2D(GLenum format, GLenum internal_format, GLsizei width, GLsizei height)
{
    colorTex = CreateEmptyTex(internal_format, format, width, height);
    depthTex = CreateEmptyTex(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, width, height);

    GLenum fboStatus;

    glGenFramebuffers(1, &fbo); GL_CHECK_ERRORS;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); GL_CHECK_ERRORS;

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTex, 0); GL_CHECK_ERRORS;
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0); GL_CHECK_ERRORS;

    fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GLuint RenderTexture2D::CreateEmptyTex(GLint internal_format, GLenum format, GLsizei width, GLsizei height)
{
    GLuint tex;

    glGenTextures(1, &tex); GL_CHECK_ERRORS;
    glBindTexture(GL_TEXTURE_2D, tex); GL_CHECK_ERRORS;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  GL_CHECK_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GL_CHECK_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  GL_CHECK_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  GL_CHECK_ERRORS;

    if (internal_format == GL_RGBA32F)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_FLOAT, NULL); GL_CHECK_ERRORS;
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL); GL_CHECK_ERRORS;
    }
    
    return tex;
}

RenderTexture2D::~RenderTexture2D()
{
    glDeleteTextures(1, &colorTex);
    glDeleteTextures(1, &depthTex);

    glDeleteFramebuffers(1, &fbo);
}

void RenderTexture2D::StartRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); GL_CHECK_ERRORS;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); GL_CHECK_ERRORS;
}

void RenderTexture2D::StopRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); GL_CHECK_ERRORS;
}

void bindTexture(const ShaderProgram& program, int unit, const std::string& name, RenderTexture2D& texture)
{
    glActiveTexture(GL_TEXTURE0 + unit);  GL_CHECK_ERRORS;
    glBindTexture(GL_TEXTURE_2D, texture.GetColorTexId());  GL_CHECK_ERRORS;

    program.SetUniform(name, unit); GL_CHECK_ERRORS;
}