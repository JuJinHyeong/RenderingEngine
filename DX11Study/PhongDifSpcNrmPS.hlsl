#include "LightVectorData.hlsl"
#include "ShaderOps.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCbuf
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D normalMap;
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    float4 diffuseSample = tex.Sample(splr, tc);
#ifdef BACK_FACE
    clip(diffuseSample.a < 0.1f ? -1 : 1);
    if(dot(viewNormal, viewPos) >= 0.0f){
        viewNormal = -viewNormal;
    }
#endif
    viewNormal = normalize(viewNormal);
    if (useNormalMap)
    {
        viewNormal = MapNormal(viewTan, viewBitan, viewNormal, tc, normalMap, splr);
    }
    
    const LightVectorData lightData = CalculateLightVectorData(viewLightPos, viewPos);
    const float att = Attenuate(attConst, attLin, attQuad, lightData.distToLight);
    const float diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightData.dirToLight, viewNormal);
    const float4 specularSample = spec.Sample(splr, tc);
    float3 specularReflectionColor = useSpecularMap ? specularSample.rgb :specularColor;
    float specularGlossLoaded = useGlossAlpha ? pow(2.0f, specularSample.a * 13.0f) : specularGloss;
    const float specular = Speculate(specularReflectionColor, specularWeight, viewNormal, lightData.dirToLight, viewPos, att, specularGlossLoaded);
    return float4(saturate((diffuse + ambient) * diffuseSample.rgb + specular), diffuseSample.a);
}