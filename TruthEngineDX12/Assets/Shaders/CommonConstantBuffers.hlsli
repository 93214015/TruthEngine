#ifndef HLSL_COMMONCONSTANTS

#define HLSL_COMMONCONSTANTS


struct Constants_Unfrequent
{
    float4 perspectiveValues;
};

struct Constants_PerFrame
{
    row_major matrix ViewInv;
    
    float3 EyePosW;
    float CBPerFramePad0;
};

#endif