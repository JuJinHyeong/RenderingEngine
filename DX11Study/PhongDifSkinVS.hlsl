#include "Transform.hlsli"
#include "VSShadow.hlsli"

cbuffer BoneOffset : register(b2)
{
    matrix boneTransforms[32];
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
    //matrix identity = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    matrix boneTransform = (matrix) 0.0f;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        boneTransform += boneTransforms[boneIndex[i]] * boneWeight[i];
    }
    float4 bonePos = mul(float4(pos, 1.0f), boneTransform);
    vso.viewPos = (float3) mul(bonePos, modelView);
    vso.viewNormal = mul(normal, (float3x3) modelView);
    vso.pos = mul(bonePos, modelViewProj);
    vso.tc = tc;
    vso.shadowHomoPos = ToShadowHomoSpace(pos, model);
    return vso;
}