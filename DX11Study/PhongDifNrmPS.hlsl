#include "PointLight.hlsli"
#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
// specular map is register to slot 1
Texture2D normalMap : register(t2);
SamplerState splr : register(s0);

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    const float shadowLevel = Shadow(spos);
    if (shadowLevel > 0.0f)
    {
        viewNormal = normalize(viewNormal);
        if (useNormalMap)
        {
            const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, normalMap, splr);
            viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
        }
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal) * shadowLevel;
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.dirToLight, viewPos, att, specularGloss) * shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}