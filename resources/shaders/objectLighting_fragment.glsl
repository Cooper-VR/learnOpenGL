#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 4

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;

float shininess;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec3 FragPosView;
in vec3 NormalView;
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform vec3 rimColor;
uniform float rimStrength;
uniform float rimPower;
uniform float rimMin;
uniform float rimMax;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = vec4(result, 1.0);



    //FragColor = vec4(normView, 1.0);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);
    diff = round(diff);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    spec = round(spec);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    vec3 normView = normalize(NormalView);
    vec3 viewDirView = normalize(-FragPosView);
    float rim = pow(1.0 - max(dot(normView, viewDirView), 0.0), rimPower) * rimStrength;
    //float rim = clamp(round(pow(1.0 - max(dot(normView, viewDirView), 0.0), 1) * 1), 0, 0.1);

    if (rim < rimMin) rim = 0;
    if (rim > rimMax) rim = rimMax;
    
    //rim = 1.0 - rim;

    rim *= diff;


    diffuse += rim;

    //return (diffuse - ambient);
    
    return (ambient + diffuse + specular);
}