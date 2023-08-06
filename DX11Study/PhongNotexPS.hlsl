#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
    float padding[3];
};

float4 main(float3 viewPos : Position, float3 viewNormal : Normal) : SV_Target
{
    viewNormal = normalize(viewNormal);
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
    const float3 specular = Speculate(diffuseColor, diffuseIntensity, viewNormal, lv.dirToLight, viewPos, att, specularPower);
    return float4(saturate(diffuse + ambient) * materialColor.rgb + specular, 1.0f);
}