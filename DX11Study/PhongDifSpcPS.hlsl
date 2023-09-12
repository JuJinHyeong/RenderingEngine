#include "LightVectorData.hlsl"
#include "ShaderOps.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCbuf
{
    bool useGlossAlpha;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
Texture2D spec;
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_TARGET
{
    viewNormal = normalize(viewNormal);
    const LightVectorData lightData = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lightData.distToLight);
    const float diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightData.dirToLight, viewNormal);
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    float3 specularReflectionColor = specularSample.rgb * specularWeight;
    if (useGlossAlpha)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }
    const float specular = Speculate(specularReflectionColor, specularWeight, viewNormal, lightData.dirToLight, viewPos, att, specularPowerLoaded);
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}