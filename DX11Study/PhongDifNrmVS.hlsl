#include "Transform.hlsli"
#include "VSShadow.hlsli"

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 viewTan : Tangent;
    float3 viewBitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowCamScreen : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.viewTan = mul(tan, (float3x3) modelView);
    vso.viewBitan = mul(bitan, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    vso.shadowCamScreen = ToShadowScreenSpace(pos, model);
    return vso;
}