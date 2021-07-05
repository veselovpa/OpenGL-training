#version 330 core

in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 outColor;

struct Material {
    int tex;
	vec3 diffuse_color;
    vec3 specular_color;
    float shininess;
};

struct PointLight {
	vec3 position;
	vec3 color;
};

struct Light {
	vec3 dir;
	vec3 color;
};

uniform Material mat;
uniform vec3 camPos;
uniform	sampler2D texture_dif;

#define POINT_LIGHTS_NUM 2
uniform PointLight lightPos[POINT_LIGHTS_NUM];
#define LIGHTS_NUM 2
uniform Light light[LIGHTS_NUM];

vec3 CalcDirLight(Light light, vec3 normal, Material mat)
{
    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = mat.diffuse_color * diff ;
    return light.color * diffuse;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 camPos, Material mat)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 viewDir = normalize(camPos - vFragPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), mat.shininess/3);
    vec3 diffuse = mat.diffuse_color * diff;
    vec3 specular = mat.specular_color * spec;

    return light.color * (diffuse + specular) * 1.5f;
}

void main()
{    
    vec3 colorPoint = vec3(0.0f, 0.0f, 0.0f);
    vec3 colorLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 2; ++i)
        colorPoint += CalcPointLight(lightPos[i], vNormal, vFragPosition, camPos, mat);

    for (int i = 0; i < 2; ++i)
        colorLight += CalcDirLight(light[i], vNormal, mat);
    
    result = colorPoint * 0.8f + colorLight * 0.5f;

    if (mat.tex == 1)
    {
        outColor = texture(texture_dif, vTexCoords) * vec4(result, 1.0f);
    }
    else 
    {
        outColor = vec4(result, 1.0f);
    }

}