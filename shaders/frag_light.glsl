#version 330 core
out vec4 color;

uniform vec3 colorLight;

void main()
{
    color = vec4(colorLight, 1.0f);
}