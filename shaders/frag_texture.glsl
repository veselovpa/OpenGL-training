#version 330 core
in vec2 vTexCoords;
in vec3 vFragPosition;
in vec3 vNormal;
in mat3 TBN;

out vec4 outColor;

struct PointLight {
	vec3 position;
	vec3 color;
};

struct Light {
	vec3 dir;
	vec3 color;
};

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform vec3 camPos;
uniform int idx;

#define POINT_LIGHTS_NUM 2
uniform PointLight lightPos[POINT_LIGHTS_NUM];
#define LIGHTS_NUM 2
uniform Light light[LIGHTS_NUM];

vec3 CalcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(normal, lightDir), 0.0);

    return light.color * diff;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 camPos)
{
    vec3 norm = normalize(normal);

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 10);

    return light.color * (diff + spec);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
//    float height =  texture(depthMap, texCoords).r;    
//    vec2 p = viewDir.xy / viewDir.z * (height * 0.1f);
//    return texCoords - p;    


    // number of depth layers
    const float minLayers = 8.0f;
    const float maxLayers = 32.0f;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * 0.1f; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    return currentTexCoords;



}

void main()
{    

    vec3 viewDir   = normalize(TBN * camPos - TBN * vFragPosition);
    // получить смещенные текстурные координаты с помощью Parallax Mapping
    vec2 texCoords = ParallaxMapping(vTexCoords,  viewDir);

    // делаем выборку из использующихся текстур 
    // с использованием смещенных координат
    vec3 diffuse = texture(diffuseMap, texCoords).rgb;
    //vec3 normal  = texture(normalMap, texCoords).rgb;
    //normal = normalize(normal * 2.0 - 1.0);

    vec3 N = vNormal;    
    N = texture(normalMap, texCoords).rgb;
    N = normalize(N * 2.0f - 1.0f);
    N = normalize(TBN * N);

    vec3 colorPoint = vec3(0.0f, 0.0f, 0.0f);
    vec3 colorLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 2; ++i)
        colorPoint += CalcPointLight(lightPos[i], N, vFragPosition, camPos);

    for (int i = 0; i < 2; ++i)
        colorLight += CalcDirLight(light[i], N);
    
    result = colorPoint * 0.8f + colorLight * 0.2f;

    outColor = texture(diffuseMap, texCoords) * vec4(result, 1.0f);
}    