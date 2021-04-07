
//
//Data Structs
//


///////////////////////////////////////////////////
//////////////// Constant Buffers
///////////////////////////////////////////////////


cbuffer CBEnvironment : register(b0)
{
    float3 gEnvironmentMapMultiplier;
    float _CBEnvironmentPad;

}

cbuffer CBPerFrame : register(b1)
{
    row_major matrix viewProj;
    
    float3 EyePos;
    float pad;

    row_major matrix gCascadedShadowTransform[4];
}

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
TextureCube tEnvironmentCubeMap : register(t0, space0);


///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_linear : register(s0);
sampler sampler_point_borderBlack : register(s1);
sampler sampler_point_borderWhite : register(s2);
SamplerComparisonState samplerComparison_great_point_borderWhite : register(s3);


struct VertexInput
{
    float3 position : POSITION;
};

struct VertexOutput
{
    float4 PosH : SV_Position;
    float3 PosL : POSITION;
};


VertexOutput vs(VertexInput vin)
{
    VertexOutput vout;
    vout.PosL = vin.position;
        
    float3 posW = vin.position * float3(5000.0, 5000.0, 5000.0);
    posW += EyePos;
    vout.PosH = mul(float4(posW, 1.0f), viewProj).xyww;

    return vout;
}

float4 ps(VertexOutput pin) : SV_Target
{    
    return tEnvironmentCubeMap.Sample(sampler_linear, pin.PosL) * float4(gEnvironmentMapMultiplier, 1.0f);

}
