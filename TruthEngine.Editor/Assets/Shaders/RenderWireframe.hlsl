cbuffer CBPerObject_Wireframe : register(b0)
{
    row_major matrix gWVP;
};

struct VertexInput
{
    float3 Position : POSITION;
};

float4 vs( VertexInput _Input) : SV_POSITION
{
    
    return mul(float4(_Input.Position, 1.0), gWVP);
}

float4 ps(float4 Position : SV_Position) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}