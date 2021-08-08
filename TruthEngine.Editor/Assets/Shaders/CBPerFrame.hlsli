cbuffer CBPerFrame : register(REGISTER_CBPerFrame)
{
    row_major matrix viewProj;
    
    float3 EyePos;
    float pad;

    row_major matrix gCascadedShadowTransform[4];
}