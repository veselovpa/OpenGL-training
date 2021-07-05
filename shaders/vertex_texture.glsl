#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;

out vec2 vTexCoords;
out vec3 vFragPosition;
out vec3 vNormal;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0f);

    vTexCoords = texCoords;
    vFragPosition = vec3(model * vec4(vertex, 1.0f));

    vNormal = normalize(mat3(transpose(inverse(model))) * normal.xyz);

    vec3 vTangent = normalize(vec3(model * vec4(tangent, 1.0f)));
    vec3 vBitangent = normalize(cross(vNormal, vTangent));

    TBN = mat3(vTangent, vBitangent, vNormal);
}