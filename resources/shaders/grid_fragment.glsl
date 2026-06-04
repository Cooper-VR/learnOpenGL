#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_specular2;  
    float shininess;
}; 

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
in vec3 WorldPos;
  
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;

void main()
{
    vec3 pos = WorldPos;
    if (abs(pos.x - round(pos.x)) < 0.03 && abs(pos.y - round(pos.y)) < 0.03 && abs(pos.z - round(pos.z)) < 0.03) {
        pos = vec3(1.0);
    }
    else {
        pos = vec3(0.3);
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);


    vec3 lightDir = normalize(-dirLight.direction);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = dirLight.ambient;

    vec3 result = ambient * diff * spec;


    FragColor = vec4(result*pos, 1.0); // set all 4 vector values to 1.0
}