#version 330 core
layout (location = 0) in vec4 vertex;
layout(location = 1) in vec4 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 vTexCoords;
out vec3 vFragPosition;
out vec3 vNormal;

uniform mat4 projection;
uniform mat4 view;

void main()
{
  vTexCoords = texCoords;
  vFragPosition = vec3(aInstanceMatrix * vertex);
  vNormal = normalize(mat3(transpose(inverse(aInstanceMatrix))) * normal.xyz);

  gl_Position = projection * view * aInstanceMatrix * vertex;
}