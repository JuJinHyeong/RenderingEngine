struct LightVectorData
{
    float3 vectorToLight;
    float3 dirToLight;
    float distToLight;
};

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 fragPos)
{
    LightVectorData lv;
    lv.vectorToLight = lightPos - fragPos;
    lv.distToLight = length(lv.vectorToLight);
    lv.dirToLight = lv.vectorToLight / lv.distToLight;
    return lv;
}