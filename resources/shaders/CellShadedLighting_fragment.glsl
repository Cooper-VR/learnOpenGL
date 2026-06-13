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

in vec3 FragPos;  
in vec3 Normal;  
in vec3 FragPosView;
in vec3 NormalView;
in vec2 TexCoords;

uniform vec3 diffuseTint;
uniform vec3 viewPos;
uniform vec3 direction;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 rimColor;
uniform float rimStrength;
uniform float rimPower;
uniform float rimMin;
uniform float rimMax;

vec3 CalcDirLight(vec3 direction, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(direction, norm, viewDir);

    FragColor = vec4(result, 1.0);



    //FragColor = vec4(normView, 1.0);
} 

vec3 CalcDirLight(vec3 direction, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-direction);

    float diff = max(dot(normal, lightDir), 0.0);
    diff = round(diff);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    spec = round(spec);

    vec3 Tint = clamp(diffuseTint, 0, 1);

    vec3 ambient = ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = diffuse * diff * (vec3(texture(texture_diffuse1, TexCoords)) * Tint);
    vec3 specular = specular * spec * vec3(texture(texture_specular1, TexCoords));

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