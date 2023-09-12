#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

#include "Transform.hlsl"

Texture2D tex;
// specular map is register to slot 1
Texture2D normalMap : register(t2);
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    viewNormal = normalize(viewNormal);
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, normalMap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
    const float3 specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.dirToLight, viewPos, att, specularGloss);
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}