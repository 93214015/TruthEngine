#ifndef HLSL_HELPERFUNCTIONS

#define HLSL_HELPERFUNCTIONS

//
//Helper Functions
//


float ConvertToLinearDepth_Perspective(float depth, float perspectiveValueW, float perspectiveValueZ)
{
    return perspectiveValueW / (depth - perspectiveValueZ);

}


float ConvertToLinearDepth_Orthographic(float depth, float perspectiveValueW, float perspectiveValueZ)
{
    return (depth - perspectiveValueW) / perspectiveValueZ;

}


float3 CalcWorldPos(float3 posV, row_major float4x4 ViewInv)
{
    float4 posW = mul(float4(posV, 1.0f), ViewInv);
    return posW.xyz;
}


float3 CalcViewPos(float2 posH, float viewDepth, float4 PerspectiveValues)
{
    float3 posV;
    posV.xy = posH.xy * PerspectiveValues.xy * viewDepth;
    posV.z = viewDepth;

    return posV.xyz;
}

#endif