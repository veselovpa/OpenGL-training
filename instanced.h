#ifndef INSTANCED_H
#define INSTANCED_H

#include <glad/glad.h>

#include "ShaderProgram.h"
#include "filysystem.h"
#include "model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

unsigned int Instanced(Model inst, float ang)
{

    float x, y, z;
    float scale = 0.15f, ans;
    int k = 1;
    unsigned int amount = 10;
    float4x4* modelMatrices;
    modelMatrices = new float4x4[amount];
    for (unsigned int i = 0; i < amount; i++)
    {
        if (i < 5)
        {
            x = (i % 5) * 15.0f - 15.0f + ang;
            if (x + 15.0f > 75.0f)
            {
                x = x - 75.0f * (int)((x + 15.0f) / 75);
            }
            z = 0.0f;
        }
        else
        {
            x = (i % 5) * 15.0f - ang;
            if (x - 60.0f < -75.0f)
            {
                x = x + 75.0f * abs((int)((x - 60.0f) / 75));
            }
            z = -50.0f;
        }
        y = 0.0f;

        if ((i % 2) == 0)
        {
            ans = ang / 100.0f;
            if (ans > 0.3f)
            {
                ans = ans - 0.3f * (int)(ans / 0.3f);
            }
            scale = abs(0.15f - ans);
        }
        else
        {
            scale = 0.15f;
        }

        float4x4 model = float4x4();        
        model = transpose(mul(translate4x4(float3(x, y, z)), scale4x4(float3(scale, scale, scale))));
        modelMatrices[i] = model;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float4x4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < inst.meshes.size(); i++)
    {
        unsigned int VAO = inst.meshes[i].VAO;
        glBindVertexArray(VAO);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)(sizeof(float4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)(2 * sizeof(float4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)(3 * sizeof(float4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
    return amount;
}

void InstancedDraw(Model inst, unsigned int am, ShaderProgram shader)
{ 

    for (unsigned int i = 0; i < inst.meshes.size(); i++)
    {
        shader.SetUniform("mat.tex", inst.material_loaded[i].tex);
        shader.SetUniform("mat.diffuse_color", inst.material_loaded[i].diffuse);
        shader.SetUniform("mat.specular_color", inst.material_loaded[i].specular);
        shader.SetUniform("mat.shininess", inst.material_loaded[i].shininess);

        for (unsigned int j = 0; j < inst.meshes[i].textures.size(); j++)
        {        

            glActiveTexture(GL_TEXTURE0 + j);
            glBindTexture(GL_TEXTURE_2D, inst.meshes[i].textures[j].id);
        }
        glBindVertexArray(inst.meshes[i].VAO);
        glDrawElementsInstanced(GL_TRIANGLES, inst.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, am);
        glBindVertexArray(0);
    }
}
#endif