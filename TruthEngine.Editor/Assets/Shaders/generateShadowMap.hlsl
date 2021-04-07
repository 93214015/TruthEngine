cbuffer perMesh : register(b0)
{
    row_major matrix gWorld;
}

cbuffer perFrame : register(b1)
{
    row_major matrix gViewProj;
}

cbuffer cb_boneTransforms : register(b2)
{
    row_major matrix gBoneTransformations[256];
};

#ifdef MESH_TYPE_SKINNED

struct VertexInput
{
    float3 position : POSITION;
    float3 BoneWeights      : BONEWEIGHT;
    uint4 BoneIndex          : BONEINDEX;
};

#else

struct VertexInput
{
    float3 position : POSITION;
};

#endif

float4 vs(VertexInput vin) : SV_POSITION
{
    
#ifdef MESH_TYPE_SKINNED

    float weightSum = vin.BoneWeights[0] + vin.BoneWeights[1] + vin.BoneWeights[2];

    float weight4 = 1.0 - weightSum;

    float4 pos = vin.BoneWeights[0] * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[0]]);
    pos += vin.BoneWeights[1] * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[1]]);
    pos += vin.BoneWeights[2] * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[2]]);
    pos += weight4 * mul(float4(vin.position, 1.0f), gBoneTransformations[vin.BoneIndex[3]]);
    pos.w = 1.0f;
    
#else
    
    float4 pos = float4(vin.position, 1.0f);
    
#endif
    
    float4 posW = mul(pos, gWorld);
    posW = mul(posW, gViewProj);
    return posW;
}