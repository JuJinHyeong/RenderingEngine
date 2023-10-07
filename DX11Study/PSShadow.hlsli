Texture2D smap : register(t3);
SamplerState ssam : register(s1);

bool ShadowUnoccluded(float4 shadowScreenPos)
{
    shadowScreenPos.xyz = shadowScreenPos.xyz / shadowScreenPos.w;
    return shadowScreenPos.z > 1.0f ? true : smap.Sample(ssam, shadowScreenPos.xy).r > shadowScreenPos.z - 0.005f;
}