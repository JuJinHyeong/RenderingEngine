#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    const float shadowLevel = Shadow(spos);
    if (shadowLevel > 0.0f)
    {
        viewNormal = normalize(viewNormal);
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal) * shadowLevel;
        specular = Speculate(
            diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
            lv.vectorToLight, viewPos, att, specularGloss
        ) * shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
    return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}