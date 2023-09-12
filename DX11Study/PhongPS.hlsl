#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

float4 main(float3 viewPos : Position, float3 viewNormal : Normal) : SV_Target
{
    viewNormal = normalize(viewNormal);
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
    const float3 specular = Speculate(
        diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
        lv.vectorToLight, viewPos, att, specularGloss
    );
    return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}