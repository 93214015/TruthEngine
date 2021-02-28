

cbuffer CBPerObject
{
    row_major matrix gWorld;
    
    float4 gExtents;
    float4 gCenter;
};

//struct VertexIn
//{
//    float3 PosL : POSITION;
//};

struct SVertexOut
{
    float4 Center : POSITION[0],
    float4 Extents : POSITION[1]
};

SVertexOut vs() : POSITION
{
    SVertexOut vOut;

    vOut.Center = gCenter;
    vOut.Extents = gExtents;

    return vOut;
}

[maxVertexcount(8)]
void gs(SVertexOut gin) : SV_Position
{
    
}

float4 ps(float4 posW : SV_Position) : SV_Target
{
    return float4(0.5f, 0.8f, 0.25f, 1.0f);
}