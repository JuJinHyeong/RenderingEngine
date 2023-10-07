float3 MapNormal(
    const float3 viewTan,
    const float3 viewBitan,
    const float3 viewNormal,
    const float2 tc,
    uniform Texture2D normalMap,
    uniform SamplerState splr
)
{
    const float3x3 tanToView = float3x3(viewTan, viewBitan, viewNormal);
    const float3 normalSample = normalMap.Sample(splr, tc).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
    // bring normal to tangent space to view space
    return normalize(mul(tanNormal, tanToView));
}

float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, uniform float distToLight)
{
    return 1.0f / (attConst + attLin * distToLight + attQuad * (distToLight * distToLight));
}

float3 Diffuse(
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float att,
    const in float3 viewDirToLight,
    const in float3 viewNormal
)
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirToLight, viewNormal));
}

float3 Speculate(
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewDirToLight,
    const in float3 viewPos,
    const in float att,
    const in float specularPower
)
{
    const float3 w = viewNormal * dot(viewDirToLight, viewNormal);
    const float3 r = w * 2.0f - viewDirToLight;
    //const float3 r = reflect(-viewDirToLight, viewNormal);
    const float3 viewCamToFrag = normalize(viewPos);
    return att * specularColor * specularIntensity * pow(max(0.0f, dot(-r, viewCamToFrag)), specularPower);
}