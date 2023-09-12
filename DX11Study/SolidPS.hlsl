// register to slot 1.
// conflict with phongPS constant buffer slot0.
cbuffer CBuf : register(b1)
{
    float3 color;
};

float4 main() : SV_Target
{
    return float4(color, 1.0f);
}