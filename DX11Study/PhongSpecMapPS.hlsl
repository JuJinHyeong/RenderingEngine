#include "PointLight.hlsli"
#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

cbuffer ObjectCBuf
{
    float specularPowerConst;
    bool hasGloss;
    float specularMapWeight;
    float padding;
};

Texture2D tex;
Texture2D spec;
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    viewNormal = normalize(viewNormal);
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
	// reflected light vector
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    float specularPower = specularPowerConst;
    if (hasGloss)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lv.dirToLight, viewPos, att, specularPower);
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected, 1.0f);
}