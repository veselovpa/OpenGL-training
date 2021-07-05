#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include "ShaderProgram.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
    float3 Position;
    float3 Normal;
    float2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float shininess;
};

struct Material {
    int tex;
    float3 diffuse;
    float3 specular;
    float shininess;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    vector<Material> material;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, vector<Material> material);

    void Draw(ShaderProgram shader);

private:

    unsigned int VBO, EBO;

    void setupMesh();
};
#endif

