#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

float LinearizeDepth(float depth);

uniform float near = 0.005; 
uniform float far  = 1000.0; 
uniform vec3 fogColor = vec3(0.5, 0.5, 0.5);


void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    float depth = LinearizeDepth(texture(depthTexture, TexCoords).r);

    depth = clamp(depth, 0.0, 1.0);
    //FragColor = vec4(vec3(depth), 1.0);

    if (depth > 0.9999) 
    {
        depth = 0.0; //should remove skybox from being affected
    }
    depth = clamp(depth * 10, 0.0, 1.0);

    //FragColor = vec4(vec3(depth), 1.0);
    FragColor = vec4(mix(col, fogColor, depth), 1.0);
    
} 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}