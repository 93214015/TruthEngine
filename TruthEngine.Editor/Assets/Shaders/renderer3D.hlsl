//
//Data Structs
//
struct Material
{
    float4 Diffuse;
    
    float3 FresnelR0;
    float Shininess;
    
    float2 UVScale;
    float2 UVTranslate;
    
    uint MapIndexDiffuse;
    uint MapIndexNormal;
    uint MapIndexDisplacement;
    uint pad;
};


struct CLightDirectionalData
{
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float3 Direction;
    float LightSize;
    
    bool CastShadow;
    float3 padDLight;
    
    //float3 Position;
    
    //float Range;
    //float3 padLight;
    
    //row_major matrix shadowTransform;
};


//
//Helper Funcitions
//
float ClacAttenuation(float d, float fallOffStart, float fallOffEnd)
{
    return saturate((fallOffEnd - d) / (fallOffEnd - fallOffStart));
}

//Schlik gives an approximation to fresnel reflectance
float3 SchlikFresnel(float3 r0, float3 normal, float3 lightVector)
{
    float cosIncidentAngle = saturate(dot(normal, lightVector));
    
    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);
    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat, float3 materialAlbedoColor)
{
    //Derive m from shininess which is derived from roughness.
    
    const float m = mat.Shininess * 256.0f;
    //const float m = mat.Shininess;
    float3 halfVec = normalize(toEye + lightVec);
    
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    roughnessFactor = max(roughnessFactor, 0.0f);
    
    float3 fresnelFactor = SchlikFresnel(mat.FresnelR0, normal, lightVec);
    
    //our Specular formula goes outside [0,1] but we are doing LDR rendering. so scale it down a bit
    float3 specAlbedo = fresnelFactor * roughnessFactor;
    
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);
    
    return (materialAlbedoColor + specAlbedo) * lightStrength;

}


///////////////////////////////////////////////////
//////////////// Constant Buffers
///////////////////////////////////////////////////

cbuffer per_mesh : register(b0)
{
    row_major matrix gWorld;
    row_major matrix gWorldInverseTranspose;
    
    uint materialIndex;
    float3 padPerMesh;
}

cbuffer CBPerFrame : register(b1)
{
    row_major matrix viewProj;
    
    float3 EyePos;
    float pad;

    row_major matrix gCascadedShadowTransform[4];
}

cbuffer CBLightData : register(b2)
{
    CLightDirectionalData gDLights[1];
}

cbuffer CBMaterials : register(b3)
{
    Material MaterialArray[200];
}

cbuffer CBUnfrequent : register(b4)
{
    uint gDLightCount;
    bool gEnabledEnvironmentMap;
    uint2 padCBunfrequent;
}

cbuffer cb_boneTransforms : register(b5)
{
    row_major matrix gBoneTransformations[256];
};

///////////////////////////////////////////////////
//////////////// Textures
///////////////////////////////////////////////////
Texture2D<float> tShadowMap : register(t0, space0);
TextureCube tEnvironmentMap : register(t1, space0);

Texture2D<float4> MaterialTextures[500] : register(t2, space0);


///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_linear : register(s0);
sampler sampler_point_borderBlack : register(s1);
sampler sampler_point_borderWhite : register(s2);
SamplerComparisonState samplerComparison_great_point_borderWhite : register(s3);

#ifdef MESH_TYPE_SKINNED

struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    float3 BoneWeights      : BONEWEIGHT;
    uint4 BoneIndex          : BONEINDEX;
};

#else

struct vertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
};

#endif

struct vertexOut
{
    float4 pos : SV_Position;
    float3 posW : POSITION0;
    //float4 posLight : POSITION1;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 texCoord : TEXCOORD;
};

vertexOut vs(vertexInput vin)
{
    vertexOut vout;
    
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
    vout.pos = mul(posW, viewProj);
    vout.posW = posW.xyz;
    //vout.posLight = mul(posW, shadowTransform);
    vout.normalW = mul(vin.normal, (float3x3) gWorldInverseTranspose);
    vout.tangentW = mul(vin.tangent, (float3x3) gWorld);
    vout.texCoord = vin.texCoord;
    
    
    return vout;
}

float4 ps(vertexOut pin) : SV_Target
{

    float3 normal = normalize(pin.normalW);

    Material _material = MaterialArray[materialIndex];
    float2 _texUV = (pin.texCoord * _material.UVScale) + _material.UVTranslate;
    
#ifdef ENABLE_MAP_NORMAL
        float3 tangent = normalize(pin.tangentW);
        float3 bitangent = cross(normal, tangent);
        float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    
        normal = MaterialTextures[_material.MapIndexNormal].Sample(sampler_linear, _texUV).xyz;
        normal = (normal * 2.0f) - 1.0f;
        normal = normalize(normal);
        
        normal = mul(normal, TBN);
#endif
    
    //normal = mul(normal, (float3x3)gWorldInverseTranspose);
    
    float3 illumination_albedo = _material.Diffuse.xyz;
    
#ifdef ENABLE_MAP_DIFFUSE
        illumination_albedo = MaterialTextures[_material.MapIndexDiffuse].Sample(sampler_linear, _texUV).xyz;
#endif
    
    float3 toEye = normalize(EyePos.xyz - pin.posW);

    float3 litColor = float3(.0f, .0f, .0f);
    
    for (uint i = 0; i < gDLightCount; ++i)
    {
        float3 lightVector = -1.0 * normalize(gDLights[0].Direction);
        float lightFactor = dot(lightVector, normal);
        lightFactor = max(lightFactor, 0.0f);
    
        float3 lightStrength = lightFactor * gDLights[0].Diffuse.xyz;
    
        float3 lit = BlinnPhong(lightStrength, lightVector, normal, toEye, _material, illumination_albedo);
    
        
        //float3 shadowMapCoords = pin.posLight.xyz / pin.posLight.w;

        //code for cascaded shadow map; finding corrsponding shadow map cascade and coords
        bool found = false;
        float3 shadowMapCoords = float3(0.0f, 0.0f, 0.0f);
    
        shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[0]).xyz;

        if (shadowMapCoords.x > 0.0f && shadowMapCoords.y > 0.0f && shadowMapCoords.x < 0.5f && shadowMapCoords.y < 0.5f)
        {
            found = true;
        }
    
        if (!found)
        {
            shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[1]).xyz;

            if (shadowMapCoords.x > 0.5f && shadowMapCoords.y > 0.0f && shadowMapCoords.x < 1.0f && shadowMapCoords.y < 0.5f)
            {
                found = true;
            }
        }
        if (!found)
        {
            shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[2]).xyz;

            if (shadowMapCoords.x > 0.0f && shadowMapCoords.y > 0.5f && shadowMapCoords.x < 0.5f && shadowMapCoords.y < 1.0f)
            {
                found = true;
            }
        }
        if (!found)
        {
            shadowMapCoords = mul(float4(pin.posW, 1.0f), gCascadedShadowTransform[3]).xyz;

            if (shadowMapCoords.x > 0.5f && shadowMapCoords.y > 0.5f && shadowMapCoords.x < 1.0f && shadowMapCoords.y < 1.0f)
            {
                found = true;
            }
        }

        float shadowMapSample = tShadowMap.Sample(sampler_point_borderWhite, shadowMapCoords.xy);
        float shadowFactor = (float) (shadowMapSample < shadowMapCoords.z);
    
        shadowMapSample = tShadowMap.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(0, 1));
        shadowFactor += (float) (shadowMapSample < shadowMapCoords.z);
    
        shadowMapSample = tShadowMap.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(1, 0));
        shadowFactor += (float) (shadowMapSample < shadowMapCoords.z);
    
        shadowMapSample = tShadowMap.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(1, 1));
        shadowFactor += (float) (shadowMapSample < shadowMapCoords.z);
    
        shadowFactor *= 0.25;
        //float shadowFactor = tShadowMap.SampleCmp(samplerComparison_great_point_borderWhite, shadowMapCoords.xy, shadowMapCoords.z);
    
    
        //illumination_albedo = lightFactor * (illumination_albedo * Diffuse.xyz).xyz;
        float3 illumination_ambient = gDLights[0].Ambient.xyz * illumination_albedo;
        
        litColor += (lit * shadowFactor.xxx) + illumination_ambient;
        
        
        
    }
    
    if (gEnabledEnvironmentMap)
    {
        float3 reflectVector = reflect(-toEye, pin.normalW);
        float3 evironmentReflectColor = tEnvironmentMap.Sample(sampler_linear, reflectVector).xyz;
        float3 frenselFactor = SchlikFresnel(_material.FresnelR0, pin.normalW, reflectVector);
    
        litColor.rgb += _material.Shininess * frenselFactor * evironmentReflectColor;
    }
    
    return float4(litColor, 1.0f);
}