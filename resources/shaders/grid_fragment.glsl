#version 330 core
out vec4 FragColor;

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
in vec2 TexCoords;
in vec3 WorldPos;
  
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;

uniform float near = 0.1; 
uniform float far  = 100.0; 

vec4 grid(vec3 p, float scale);
float LinearizeDepth(float depth);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-dirLight.direction);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 lighting = dirLight.ambient + 
                    dirLight.diffuse * diff + 
                    dirLight.specular * spec;

    float depth = LinearizeDepth(gl_FragCoord.z);
    vec4 gridColor = grid(FragPos, 10.0);   // grid density

    // Mix grid on top of a dark base (prevents pink flood)
    vec3 base = vec3(0.08);                  // very dark floor
    vec3 finalColor = mix(base, gridColor.rgb, gridColor.a);



    FragColor = vec4(finalColor * lighting, 1.0)* clamp(1/depth, 0, 0.4);

}

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec4 grid(vec3 p, float scale)
{
    vec2 coord = p.xz * scale;
    vec2 deriv = fwidth(coord);

    vec2 gridDist = abs(fract(coord - 0.5) - 0.5) / deriv;
    float line = min(gridDist.x, gridDist.y);
    float alpha = 1.0 - min(line, 1.0);

    // Base grid lines = light gray
    vec3 color = vec3(0.65);

    // Colored axes (thin)
    float axisThickness = 0.1;

    // Red lines (constant Z = 0)
    if (abs(p.y) < axisThickness / scale) {
        color = vec3(0.9, 0.25, 0.25);
    }
    // Blue lines (constant X = 0)
    if (abs(p.z) < axisThickness / scale) {
        color = vec3(0.9, 0.25, 0.25);
    }

    return vec4(color, alpha);
}