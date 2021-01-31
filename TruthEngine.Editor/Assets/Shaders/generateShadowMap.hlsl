
cbuffer perFrame : register(b0)
{
    row_major matrix gViewProj;
}

cbuffer perMesh : register(b1)
{
    row_major matrix gWorld;
}


float4 vs( float3 pos : POSITION ) : SV_POSITION
{
    float4 posW = mul(float4(pos, 1.0f), gWorld);
    return mul(posW,  gViewProj);
}