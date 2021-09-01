
cbuffer ConstantBuffer_Billboards : register(b0)
{
    float3 gCenter;
};

float4 main( float4 pos : POSITION ) 
{
	return pos;
}