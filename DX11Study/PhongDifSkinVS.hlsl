#include "Transform.hlsli"
#include "VSShadow.hlsli"

cbuffer BoneOffset : register(b1)
{
    const matrix boneTransforms[1000];
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float2 tc : Texcoord;
    float4 shadowHomoPos : ShadowPosition;
    float4 pos : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 normal : Normal, float2 tc : Texcoord, float4 boneIndex : BoneIndex, float4 boneWeight : BoneWeight)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(normal, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    vso.shadowHomoPos = ToShadowHomoSpace(pos, model);
    return vso;
}