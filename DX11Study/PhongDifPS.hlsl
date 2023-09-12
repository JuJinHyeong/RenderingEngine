#include "LightVectorData.hlsl"
#include "ShaderOps.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
SamplerState splr;

float4 main(float3 viewPos: Position, float3 viewNormal: Normal, float2 tc: Texcoord) : SV_TARGET
{
    viewNormal = normalize(viewNormal);
    const LightVectorData lightData = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lightData.distToLight);
    const float diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightData.dirToLight, viewNormal);
    const float specular = Speculate(specularColor, specularWeight, viewNormal, lightData.dirToLight, viewPos, att, specularGloss);
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}