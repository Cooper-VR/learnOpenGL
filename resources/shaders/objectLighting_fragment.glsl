#version 330 core
out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

struct Matrial{
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Matrial matrial;

in vec3 Normal;
in vec3 FragPos;

void main(){
    vec3 ambiant = matrial.ambiant * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * matrial.diffuse);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matrial,shininess);
    vec3 specular = lightColor * (spec * matrial.specular);

    vec3 result = (ambiant + diffuse + specular);
    fragColor = vec4(result, 1.0);
}