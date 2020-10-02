struct vertexOut {
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

struct perObject
{
    float4 diffuseColor;

    float4 ambientColor;

    float4 specularColor;
    
    int diffuseMapIndex;
    float3 padding1;
    
    float4 padding2[12];
};

cbuffer constants_frame : register(b0)
{
    matrix viewProj;
};

cbuffer constants_mesh : register(b1)
{
    int cbPerMeshIndex;
};


ConstantBuffer<perObject> cbPerObject[] : register(b2);

Texture2D<float4> tex[] : register(t0);

sampler sampler1 : register(s0);

vertexOut vs(float3 pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD, uint vertexIndex : SV_VertexID)
{
	vertexOut vout;
    vout.position = mul(float4(pos, 1.0f), viewProj);
	vout.texCoord = texCoord;
	return vout;
}




float4 ps(vertexOut pin) : SV_TARGET
{
    
    float4 color = cbPerObject[cbPerMeshIndex].diffuseColor;
    
    int diffuseMapIndex = cbPerObject[cbPerMeshIndex].diffuseMapIndex;
    
    if (diffuseMapIndex > -1)
        color *= tex[diffuseMapIndex].Sample(sampler1, pin.texCoord);
    
    return color;
    
}