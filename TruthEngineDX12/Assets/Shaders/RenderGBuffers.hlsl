

struct vertexOut
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
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


//
//Constants
//

cbuffer constants_frame : register(b0)
{
    matrix viewProj;
};


cbuffer constants_mesh : register(b1)
{
    int cbPerMeshIndex;
};


ConstantBuffer<perObject> cbPerObject[] : register(b2);



//
//Textures
//
Texture2D<float4> tex[] : register(t0);

//
//Samplers
//
sampler sampler1 : register(s0);


vertexOut vs(float3 pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD)
{
    vertexOut vout;
    vout.position = mul(float4(pos, 1.0f), viewProj);
    vout.normal = normal;
    vout.texCoord = texCoord;
    return vout;
}


struct PixelOut
{
    float4 color : SV_Target0;
    float3 normal : SV_Target1;
};


PixelOut ps(vertexOut pin)
{
    PixelOut pOut;
    pOut.color = cbPerObject[cbPerMeshIndex].diffuseColor;
    
    int diffuseMapIndex = cbPerObject[cbPerMeshIndex].diffuseMapIndex;
    
    if (diffuseMapIndex > -1)
        pOut.color *= tex[diffuseMapIndex].Sample(sampler1, pin.texCoord);
    
    pOut.normal = pin.normal;
    
    return pOut;
    
}