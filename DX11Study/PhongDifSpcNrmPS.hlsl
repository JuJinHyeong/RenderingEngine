#include "LightVectorData.hlsli"
#include "ShaderOps.hlsli"
#include "PointLight.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCbuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);
Texture2D normalMap : register(t2);
SamplerState splr : register(s0);

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_TARGET
{
    float3 diffuse;
    float3 specular;
    const float4 diffuseSample = tex.Sample(splr, tc);
#ifdef BACK_FACE
    clip(diffuseSample.a < 0.1f ? -1 : 1);
    if(dot(viewNormal, viewPos) >= 0.0f){
        viewNormal = -viewNormal;
    }
#endif
    const float shadowLevel = Shadow(spos);
    if (shadowLevel > 0.0f)
    {
        viewNormal = normalize(viewNormal);
        if (useNormalMap)
        {
            viewNormal = MapNormal(viewTan, viewBitan, viewNormal, tc, normalMap, splr);
        }
    
        const LightVectorData lightData = CalculateLightVectorData(viewLightPos, viewPos);
        const float att = Attenuate(attConst, attLin, attQuad, lightData.distToLight);
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightData.dirToLight, viewNormal) * shadowLevel;
        const float4 specularSample = spec.Sample(splr, tc);
        float3 specularReflectionColor = useSpecularMap ? specularSample.rgb : specularColor;
        float specularGlossLoaded = useGlossAlpha ? pow(2.0f, specularSample.a * 13.0f) : specularGloss;
        specular = Speculate(diffuseColor * specularReflectionColor, specularWeight, viewNormal, lightData.dirToLight, viewPos, att, specularGlossLoaded) * shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
    return float4(saturate((diffuse + ambient) * diffuseSample.rgb + specular), diffuseSample.a);
}