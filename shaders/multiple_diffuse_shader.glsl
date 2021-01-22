#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aNormal;
layout(location=2)in vec2 texCoord;
layout (location = 1) in vec3 aColor;


out vec3 FragPos;
out vec3 Normal;
out vec2 v_TexCoord;
out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int useTextures;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    if (useTextures==1){
        v_TexCoord =texCoord;
    }
    else {
        ourColor =aColor;
    }
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

    #shader fragment
    #version 410 core

    #define NR_DIFF_LIGHTS 30
out vec4 FragColor;

layout(location=0)out vec4 color;

in vec2 v_TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 ourColor;

uniform sampler2D u_Texture;
uniform int numDiffLights;
uniform int useTextures;

struct Light {
    vec3 position;
    vec3 color;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light lights[NR_DIFF_LIGHTS];
uniform Material material;
vec3 CalcDiffLight(Light light, vec3 normal, vec3 fragPos);
void main(){
    // diffuse
    vec3 norm = normalize(Normal);

    vec3 result;

    for (int i = 0; i < numDiffLights; i++)
    result += CalcDiffLight(lights[i], norm, FragPos);

    FragColor = vec4(result, 1.0);
    if (useTextures==1){
        vec4 texColor = texture(u_Texture, v_TexCoord);
        color = FragColor*texColor;
    }
    else {
        color = vec4(material.diffuse-vec3(0.2)+material.diffuse,1.0)+FragColor;
    }
}
// calculates the color when using a directional light.
vec3 CalcDiffLight(Light light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // combine results
    vec3 ambient = vec3(0.1);
    vec3 diffuse = light.color * diff;
    return (ambient + diffuse);
}