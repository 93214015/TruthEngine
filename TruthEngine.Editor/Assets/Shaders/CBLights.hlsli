cbuffer CBLights : register(REGISTER_CBLights)
{
    CLightDirectionalData gDLights[1];
    CLightSpotData gSpotLights[1];
    CLightPointData gPointLights[20];
}