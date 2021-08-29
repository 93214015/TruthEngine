

cbuffer CBPerObject
{
    float4 gExtents;
    float4 gCenter;
};

//struct VertexIn
//{
//    float3 PosL : POSITION;
//};



float vs()
{
    return 0;
}

[maxVertexcount(16)]
void gs() : SV_Position
{
    
}

float4 ps(float4 posW : SV_Position) : SV_Target
{
    return float4(0.5f, 0.8f, 0.25f, 1.0f);
}