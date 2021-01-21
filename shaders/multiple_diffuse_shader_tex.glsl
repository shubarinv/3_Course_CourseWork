#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aNormal;
layout(location=2)in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 v_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    v_TexCoord =vec2(model*vec4(vec3(texCoord,1.0),1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

    #shader fragment
    #version 410 core

    #define NR_DIFF_LIGHTS 30
out vec4 FragColor;

layout(location=0)out vec4 color;

in vec2 v_TexCoord;
uniform sampler2D u_Texture;
in vec3 Normal;
in vec3 FragPos;
uniform int numDiffLights;

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light lights[NR_DIFF_LIGHTS];

vec3 CalcDiffLight(Light light, vec3 normal, vec3 fragPos);
void main()
{
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 result;

    for(int i = 0; i < numDiffLights; i++)
    result += CalcDiffLight(lights[i], norm, FragPos);

    FragColor = vec4(result, 1.0);
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = FragColor*texColor;
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