#include "PointLight.hlsli"
#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    // bool is 4bytes in hlsl
    bool normalMapEnabled;
    float padding;
};

#include "Transform.hlsli"

Texture2D tex;
Texture2D normalMap;

SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    viewNormal = normalize(viewNormal);
    if (normalMapEnabled)
    {
        const float3 normalSample = normalMap.Sample(splr, tc).xyz;
        float3 objectNormal = normalSample * 2.0f - 1.0f;
        objectNormal.y = -objectNormal.y;
        viewNormal = normalize(mul(objectNormal, (float3x3) modelView));
    }
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
    const float3 specular = Speculate(specularIntensity.rrr, 1.0f, viewNormal, lv.dirToLight, viewPos, att, specularPower);
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}