#include "LightVectorData.hlsli"
#include "ShaderOps.hlsli"
#include "PointLight.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spos : ShadowPosition, float4 color : Color) : SV_TARGET
{
    float3 diffuse;
    float3 specular;
    
    const float shadowLevel = Shadow(spos);
    if (shadowLevel > 0.0f)
    {
        viewNormal = normalize(viewNormal);
        const LightVectorData lightData = CalculateLightVectorData(viewLightPos, viewPos);
        const float att = Attenuate(attConst, attLin, attQuad, lightData.distToLight);
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightData.dirToLight, viewNormal) * shadowLevel;
        specular = Speculate(diffuseColor * specularColor, specularWeight, viewNormal, lightData.dirToLight, viewPos, att, specularGloss) * shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
    
    float4 test = (color.r + color.g + color.b) == 0.0f ? tex.Sample(splr, tc) : color;
    
    return float4(saturate((diffuse + ambient) * test.rgb + specular), 1.0f);
}