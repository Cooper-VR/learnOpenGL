#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

uniform bool noFog;

float LinearizeDepth(float depth);

uniform float near = 0.005; 
uniform float far  = 1000.0; 
uniform vec3 fogColor = vec3(0.5, 0.5, 0.5);


void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    float depth = LinearizeDepth(texture(depthTexture, TexCoords).r);

    depth = clamp(depth, 0.0, 1.0);
    //depth = depth / far;


    vec3 outColor;

    if (depth == 1.0) depth = 0.0;

    depth = clamp(depth*1.3, 0.0, 1.0);

    if (noFog)
        FragColor = vec4(col, 1.0);
    else
        FragColor = vec4(mix(col, fogColor, depth), 1.0);

} 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}