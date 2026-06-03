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

uniform float _starHeight;
uniform float _starPower;
uniform float _starDetailPower;
uniform float _densityMapIntensity;
uniform float _starIntensity;
uniform vec3 _ExtraStarDetail;
uniform float _starDetailScale;

uniform float _AuroraPower;
uniform float _AuroraOffset;
uniform float _AuroraNoiseScale;
uniform float _AuroraIntensity;
uniform vec2 _AuroraTiling;
uniform float _AuroraBlend;
uniform vec2 _AuroraScrollSpeed;
uniform float _twirlStrngth;
uniform vec2 _twirlOffset;
uniform float _distortionSpeed;
uniform vec2 _distortionMinMax;

uniform vec3 _moonDirection;
uniform float _moonSize;
uniform vec3 _moonColor;
uniform vec3 _moonUp;
uniform vec3 _moonRight;
uniform float _rotation;
uniform float _moonPhase;
uniform float _moonIntensity;

uniform vec2 _StarDensityMap_Tiling;
uniform vec2 _StarDensityMap_Offset;
uniform vec2 _StarNoiseTexture_Tiling;
uniform vec2 _StarNoiseTexture_Offset;
uniform vec2 _StarNormalMap_Tiling;
uniform vec2 _StarNormalMap_Offset;
uniform vec3 _AuroraNoiseGradient;
uniform vec3 _AuroraColorGradient;

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

vec3 sampleStarGradient(float t)
{
    vec3 c0 = vec3(0.9254902, 0.9019608, 0.9725490);
    vec3 c1 = vec3(0.95, 0.93, 1.00);
    vec3 c2 = vec3(1.00, 0.94, 1.00);
    float a = smoothstep(0.0, 0.55, t);
    float b = smoothstep(0.55, 1.0, t);
    return mix(mix(c0, c1, a), c2, b);
}

vec3 sampleAuroraGradient(float t)
{
    vec3 a = (length(_AuroraNoiseGradient) > 0.0001) ? _AuroraNoiseGradient : vec3(0.9411765, 0.9411765, 0.9411765);
    vec3 b = vec3(0.0, 1.0, 1.0);
    vec3 c = (length(_AuroraColorGradient) > 0.0001) ? _AuroraColorGradient : vec3(1.0, 0.0, 1.0);
    return mix(mix(a, b, smoothstep(0.0, 0.6, t)), c, smoothstep(0.6, 1.0, t));
}

void main()
{
    float zenithBlend = (_ZenithBlend > 0.0) ? _ZenithBlend : 0.93;
    float nadirBlend = (_NaDirBlend > 0.0) ? _NaDirBlend : 42.5;
    float horizonBlend = (_HorizonBlend > 0.0) ? _HorizonBlend : 5.0;
    vec3 skyColorUniform = (length(_SkyColor) > 0.0001) ? _SkyColor : vec3(0.2470588, 0.0, 0.3803922);
    vec3 horizonColorUniform = (length(_HorizonColor) > 0.0001) ? _HorizonColor : vec3(0.4352941, 0.0156863, 0.5254902);
    vec3 groundColorUniform = (length(_groundColor) > 0.0001) ? _groundColor : vec3(0.6352941, 0.0509804, 0.6039216);

    float starHeight = (_starHeight > 0.0) ? _starHeight : 53.4;
    float starPower = (_starPower > 0.0) ? _starPower : 883.7;
    float starDetailPower = (_starDetailPower > 0.0) ? _starDetailPower : 0.4;
    float densityMapIntensity = (_densityMapIntensity > 0.0) ? _densityMapIntensity : 50.68;
    float starIntensity = (_starIntensity > 0.0) ? _starIntensity : 190.0;
    vec3 extraStarDetail = (length(_ExtraStarDetail) > 0.0001) ? _ExtraStarDetail : vec3(0.05490196, 0.1411765, 0.6901961);
    float starDetailScale = (_starDetailScale > 0.0) ? _starDetailScale : 0.31;

    float auroraPower = (_AuroraPower > 0.0) ? _AuroraPower : 6.41;
    float auroraOffset = (_AuroraOffset != 0.0) ? _AuroraOffset : 1.0;
    float auroraNoiseScale = (_AuroraNoiseScale > 0.0) ? _AuroraNoiseScale : 3.65;
    float auroraIntensity = (_AuroraIntensity > 0.0) ? _AuroraIntensity : 2.3;
    vec2 auroraTiling = (length(_AuroraTiling) > 0.0001) ? _AuroraTiling : vec2(1.29, 0.94);
    float auroraBlend = (_AuroraBlend > 0.0) ? _AuroraBlend : 1.0;
    vec2 auroraScroll = (length(_AuroraScrollSpeed) > 0.0001) ? _AuroraScrollSpeed : vec2(0.08, 0.07);
    float twirlStrength = (_twirlStrngth > 0.0) ? _twirlStrngth : 1.04;
    vec2 twirlOffset = (length(_twirlOffset) > 0.0001) ? _twirlOffset : vec2(-2.85, 3.98);
    float distortionSpeed = (_distortionSpeed > 0.0) ? _distortionSpeed : 0.06;
    vec2 distortionRange = (length(_distortionMinMax) > 0.0001) ? _distortionMinMax : vec2(0.03, 3.3);

    vec3 moonDirection = (length(_moonDirection) > 0.0001) ? _moonDirection : normalize(vec3(-0.2836898, -0.4965194, 0.8203588));
    float moonSize = (_moonSize > 0.0) ? _moonSize : 0.16;
    vec3 moonColorUniform = (length(_moonColor) > 0.0001) ? _moonColor : vec3(0.93, 0.96, 1.00);
    vec3 moonUpUniform = _moonUp;
    vec3 moonRightUniform = _moonRight;
    float moonRotation = _rotation;
    float moonPhase = clamp(_moonPhase, 0.0, 1.0);
    float moonIntensity = (_moonIntensity > 0.0) ? _moonIntensity : 1.0;

    vec2 starDensityTiling = (length(_StarDensityMap_Tiling) > 0.0001) ? _StarDensityMap_Tiling : vec2(-0.04, 0.08);
    vec2 starDensityOffset = _StarDensityMap_Offset;
    vec2 starNoiseTiling = (length(_StarNoiseTexture_Tiling) > 0.0001) ? _StarNoiseTexture_Tiling : vec2(0.57, 0.29);
    vec2 starNoiseOffset = _StarNoiseTexture_Offset;
    vec2 starNormalTiling = (length(_StarNormalMap_Tiling) > 0.0001) ? _StarNormalMap_Tiling : vec2(100.8, 1.0);
    vec2 starNormalOffset = _StarNormalMap_Offset;

    vec3 dir = normalize(WorldPos - viewPos);
    float up = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);

    // Sky and ground gradient.
    float zenithMix = pow(max(up, 0.0), max(zenithBlend, 0.001));
    float nadirMix = pow(max(1.0 - up, 0.0), max(nadirBlend, 0.001));
    float horizonBand = pow(1.0 - abs(dir.y), max(horizonBlend, 0.001));
    vec3 skyBase = mix(groundColorUniform, skyColorUniform, zenithMix);
    vec3 horizon = horizonColorUniform * horizonBand;
    vec3 color = mix(skyBase, groundColorUniform, nadirMix * 0.3) + horizon;

    // Stars from procedural hash/noise (no texture map required).
    vec2 starUV = dir.xz / max(abs(dir.y), 0.08);
    starUV = starUV * starDensityTiling + starDensityOffset;
    vec2 starNoiseUV = starUV * starNoiseTiling + starNoiseOffset;
    vec2 starDetailUV = starUV * starNormalTiling + starNormalOffset;
    float starScale = max(starHeight, 1.0) * 0.12;
    float starField = fbm(starNoiseUV * starScale + vec2(17.0, -11.0));
    float starMask = pow(clamp(starField, 0.0, 1.0), max(starPower, 1.0));
    float detail = pow(max(valueNoise2D(starDetailUV * max(starDetailScale, 0.1)), 0.0001), max(starDetailPower, 0.001));
    vec3 starColor = sampleStarGradient(starField) * starIntensity * starMask;
    starColor += extraStarDetail * detail;
    starColor *= mix(0.2, 1.2, clamp(densityMapIntensity, 0.0, 1.0));
    starColor *= smoothstep(-0.15, 0.25, dir.y);
    color += starColor;

    // Aurora using warped noise bands.
    vec2 auroraUV = dir.xz / max(abs(dir.y), 0.08);
    float distortion = sin(uTime * distortionSpeed);
    distortion = mix(distortionRange.x, distortionRange.y, distortion * 0.5 + 0.5);
    vec2 tw = auroraUV - twirlOffset;
    float ang = twirlStrength * length(tw);
    mat2 rot = mat2(cos(ang), -sin(ang), sin(ang), cos(ang));
    tw = rot * tw + twirlOffset;
    tw = tw * auroraTiling + auroraScroll * uTime;

    float auroraNoise = fbm(tw * max(auroraNoiseScale, 0.001) + distortion);
    float auroraMask = pow(clamp(auroraNoise + auroraOffset, 0.0, 1.0), max(auroraPower, 0.001));
    vec3 aurora = sampleAuroraGradient(auroraNoise) * auroraMask * auroraIntensity * auroraBlend;
    aurora *= smoothstep(0.0, 0.6, dir.y);
    color += aurora;

    // Procedural moon disc with simple phase.
    vec3 moonF = normalize(moonDirection);
    vec3 moonU = normalize(moonUpUniform);
    vec3 moonR = normalize(moonRightUniform);
    if (length(moonU) < 0.01 || length(moonR) < 0.01)
    {
        moonU = vec3(0.0, 1.0, 0.0);
        moonR = normalize(cross(moonU, moonF + vec3(0.001, 0.0, 0.0)));
        moonU = normalize(cross(moonF, moonR));
    }

    float moonDot = clamp(dot(dir, moonF), -1.0, 1.0);
    float moonAngle = acos(moonDot);
    float moonRadius = max(moonSize, 0.0001);
    float moonMask = 1.0 - smoothstep(moonRadius * 0.95, moonRadius, moonAngle);

    vec3 local = vec3(dot(dir, moonR), dot(dir, moonU), dot(dir, moonF));
    vec2 moonUV = local.xy / moonRadius;
    mat2 moonRot = mat2(cos(moonRotation), -sin(moonRotation), sin(moonRotation), cos(moonRotation));
    moonUV = moonRot * moonUV;

    float moonDisc = 1.0 - smoothstep(0.95, 1.0, length(moonUV));
    float phaseEdge = (moonPhase * 2.0 - 1.0);
    float phaseMask = smoothstep(phaseEdge - 0.08, phaseEdge + 0.08, moonUV.x);
    vec3 moon = moonColorUniform * moonMask * moonDisc * phaseMask * moonIntensity;
    color += moon;

    color += _AuroraNoiseGradient * 0.0 + _AuroraColorGradient * 0.0;

    FragColor = vec4(max(color, 0.0), 1.0);
}

/*
void main()
{
    //FragColor = vec4(1.0, 0.0, 1.0, 1.0); // set all 4 vector values to 1.0


}*/