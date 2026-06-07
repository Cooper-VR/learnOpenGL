#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 viewPos;
uniform float uTime;

uniform float _ZenithBlend;
uniform float _NaDirBlend;
uniform float _HorizonBlend;
uniform vec3 _SkyColor;
uniform vec3 _HorizonColor;
uniform vec3 _groundColor;

float hash31(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return fract((p.x + p.y) * p.z);
}

float valueNoise2D(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);

    float a = hash31(vec3(i, 0.0));
    float b = hash31(vec3(i + vec2(1.0, 0.0), 0.0));
    float c = hash31(vec3(i + vec2(0.0, 1.0), 0.0));
    float d = hash31(vec3(i + vec2(1.0, 1.0), 0.0));

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p)
{
    float value = 0.0;
    float amp = 0.5;
    for (int i = 0; i < 4; ++i)
    {
        value += amp * valueNoise2D(p);
        p *= 2.03;
        amp *= 0.5;
    }
    return value;
}


void main()
{
    float zenithBlend = (_ZenithBlend > 0.0) ? _ZenithBlend : 0.93;
    float nadirBlend = (_NaDirBlend > 0.0) ? _NaDirBlend : 42.5;
    float horizonBlend = (_HorizonBlend > 0.0) ? _HorizonBlend : 5.0;
    vec3 skyColorUniform = (length(_SkyColor) > 0.0001) ? _SkyColor : vec3(0.2470588, 0.0, 0.3803922);
    vec3 horizonColorUniform = (length(_HorizonColor) > 0.0001) ? _HorizonColor : vec3(0.4352941, 0.0156863, 0.5254902);
    vec3 groundColorUniform = (length(_groundColor) > 0.0001) ? _groundColor : vec3(0.6352941, 0.0509804, 0.6039216);



    vec3 dir = normalize(WorldPos - viewPos);
    float up = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);

    // Sky and ground gradient.
    float zenithMix = pow(max(up, 0.0), max(zenithBlend, 0.001));
    float nadirMix = pow(max(1.0 - up, 0.0), max(nadirBlend, 0.001));
    float horizonBand = pow(1.0 - abs(dir.y), max(horizonBlend, 0.001));
    vec3 skyBase = mix(groundColorUniform, skyColorUniform, zenithMix);
    vec3 horizon = horizonColorUniform * horizonBand;
    vec3 color = mix(skyBase, groundColorUniform, nadirMix * 0.3) + horizon;

    FragColor = vec4(max(color, 0.0), 1.0);
}

/*
void main()
{
    //FragColor = vec4(1.0, 0.0, 1.0, 1.0); // set all 4 vector values to 1.0


}*/