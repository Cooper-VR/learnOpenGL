#version 330 core
out vec4 fragColor;

uniform vec3 viewPos;

struct Matrial{
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Matrial matrial;

in vec3 Normal;
in vec3 FragPos;

void main(){
    vec3 ambiant = matrial.ambiant * light.ambiant;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * matrial.diffuse);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matrial.shininess);
    vec3 specular = light.specular * (spec * matrial.specular);

    vec3 result = (ambiant + diffuse + specular);
    fragColor = vec4(result, 1.0);
}