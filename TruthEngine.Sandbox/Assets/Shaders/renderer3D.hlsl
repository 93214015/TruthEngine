
struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord :TEXCOORD;
};

float4 vs(vertexInput vin) : SV_Position
{	
    return float4(vin.position, 1.0f);
}

float4 ps( float4 pos : SV_POSITION ) : SV_Target
{
    return pos;
}