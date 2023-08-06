// register to slot 1.
// conflict with phongPS constant buffer slot0.
cbuffer CBuf : register(b1)
{
    float4 color;
};

float4 main() : SV_Target
{
    return color;
}