#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    // bool is 4bytes in hlsl
    bool normalMapEnabled;
    float padding;
};

#include "Transform.hlsl"

Texture2D tex;
// specular map is register to slot 1
Texture2D normalMap : register(t2);
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    viewNormal = normalize(viewNormal);
    if (normalMapEnabled)
    {
        const float3x3 tanToView = float3x3(
            normalize(viewTan),
            normalize(viewBitan),
            normalize(viewNormal)
        );
        const float3 normalSample = normalMap.Sample(splr, tc).xyz;
        viewNormal = normalSample * 2.0f - 1.0f;
        viewNormal.z *= -1.0f;
        viewNormal.y *= -1.0f;
        // bring normal to tangent space to view space
        viewNormal = normalize(mul(viewNormal, tanToView));
    }
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
    const float3 specular = Speculate(specularIntensity.rrr, 1.0f, viewNormal, lv.dirToLight, viewPos, att, specularPower);
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}