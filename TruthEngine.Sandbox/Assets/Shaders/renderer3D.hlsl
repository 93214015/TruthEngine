struct Material
{
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
};

cbuffer per_frame : register(b0)
{
    row_major matrix viewProj : packoffset(c0);
    float4 color : packoffset(c4.x);
}

cbuffer per_dlight : register(b1)
{
    float4x4 View;
    float4x4 ViewProj;
    float4x4 ShadowTransform;

    float4 PerpectiveValues;

    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float3 Direction;
    float LightSize;

    float3 Position;
    float zNear;

    float2 DirInEyeScreen_Horz;
    float2 DirInEyeScreen_Vert;

    float zFar;
    bool CastShadow;
    int IndexShadowMapSRV;
    float pad;
    
    float4 pad2[12];
}

cbuffer materials : register(b2)
{
    Material MaterialArray[100];
}

cbuffer per_mesh : register(b3)
{
    uint materialIndex;
}


Texture2D<float4> MaterialTextures[100];


struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct vertexOut
{
    float4 pos : SV_Position;
    float3 normalW : NORMAL;
};

vertexOut vs(vertexInput vin)
{
    vertexOut vout;
    vout.pos = mul(float4(vin.position, 1.0f), viewProj);
    vout.normalW = vin.normal;
    
    return vout;
}

float4 ps(vertexOut pin) : SV_Target
{
    float3 lightVector = -1.0 * normalize(Direction);
    float dotResult = dot(lightVector, pin.normalW);
    float lightFactor = clamp(dotResult, 0.0f, 1.0f);
    
    float3 color = lightFactor * (MaterialArray[materialIndex].Diffuse * Diffuse).xyz;
    return float4(color, 1.0f);

}