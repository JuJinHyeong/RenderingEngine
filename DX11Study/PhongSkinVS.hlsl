#include "Transform.hlsli"
#include "VSShadow.hlsli"

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 shadowHomoPos : ShadowPosition;
    float4 pos : SV_Position;
};

cbuffer BoneOffset : register(b1)
{
    const matrix boneTransforms[1000];
};

VSOut main(float3 pos : Position, float3 n : Normal, float4 boneIndex : BoneIndex, float4 boneWeight : BoneWeight)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.shadowHomoPos = ToShadowHomoSpace(pos, model);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}