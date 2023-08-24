#include "PointLight.hlsl"
#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D normalMap;
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    float4 dtex = tex.Sample(splr, tc);
    
    #ifdef BACK_FACE
    clip(dtex.a < 0.1f ? -1 : 1);
    // when back face rendering, normal need to be flipped
    if (dot(viewNormal, viewPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    #endif
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(viewTan, viewBitan, viewNormal, tc, normalMap, splr);
    }
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToLight);
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToLight, viewNormal);
    float3 specularReflectionColor = specularColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lv.dirToLight, viewPos, att, specularPower);
	// final color
    return float4(saturate(diffuse + ambient) * dtex.rgb + specularReflected, dtex.a);
}