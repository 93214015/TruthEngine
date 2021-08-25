#include "Lights.hlsli"
#include "MathHelper.hlsli"


/////////////////////////////////////////////////////////////////
//////////////// Constant Buffers
/////////////////////////////////////////////////////////////////

#define REGISTER_CBPerFrame b0
#include "CBPerFrame.hlsli"

#define REGISTER_CBLights b1
#include "CBLights.hlsli"

#define REGISTER_CBUnfrequent b2
#include "CBUnfrequent.hlsli"

/////////////////////////////////////////////////////////////////
//////////////// Textures
/////////////////////////////////////////////////////////////////

Texture2D<float4> tColor : register(t0);
Texture2D<float3> tNormal : register(t1);
Texture2D<float4> tSpecular : register(t2);
Texture2D<float> tDepth : register(t3);
TextureCube tIBLAmbient : register(t4);
TextureCube tIBLSpecular : register(t5);
Texture2D<float2> tPrecomputedBRDF : register(t6);
Texture2D<float> tSSAO : register(t7);
Texture2D<float> tShadowMap_SunLight : register(t8);
Texture2D<float> tShadowMap_SpotLight : register(t9);



/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//////////////// Samplers
/////////////////////////////////////////////////////////////////
#include "Samplers.hlsli"

//Defined For TriangleStrip Topology
static const float2 VertexPositions[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

struct VertexOut
{
    float4 PosH : SV_Position;
    float2 UV : TEXCOORD0;
    float2 PosCS : TEXCOORD1;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
    VertexOut _VOut;
    _VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
    _VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + (0.5f, 0.5f);
    _VOut.PosCS = _VOut.PosH.xy;
    
    return _VOut;
}

float4 ps(VertexOut _VOut) : SV_Target
{
    float _DepthProj = tDepth.Sample(sampler_point_wrap, _VOut.UV);

    if(_DepthProj > 0.9999f)
        discard;
    
    float _DepthView = ConvertToLinearDepth(_DepthProj, ProjectionValues.z, ProjectionValues.w);
    
    float4 _PosWorld = ReconstructWorldPosition(_VOut.PosCS, _DepthView, ProjectionValues, ViewInverse);
    
    float3 _NormalWorld = tNormal.Sample(sampler_point_wrap, _VOut.UV) * 2.0f - 1.0f;

    float4 _Color = tColor.Sample(sampler_point_wrap, _VOut.UV);

    float4 _SpecularFactors = tSpecular.Sample(sampler_point_wrap, _VOut.UV);
    
        
    float3 _ToEye = normalize(EyePos.xyz - _PosWorld.xyz);

    float3 _LitColor = float3(.0f, .0f, .0f);
    
    for (uint _DLightIndex = 0; _DLightIndex < gDLightCount; ++_DLightIndex)
    {
        
        float3 _Lit = ComputeDirectLight(gDLights[_DLightIndex], _Color.xyz, _SpecularFactors.w, _SpecularFactors.xyz, _PosWorld.xyz, _NormalWorld, _ToEye);
		
        //code for cascaded shadow map; finding corrsponding shadow map cascade and coords
        bool found = false;
        float3 shadowMapCoords = float3(0.0f, 0.0f, 0.0f);
    
        shadowMapCoords = mul(_PosWorld, gCascadedShadowTransform[0]).xyz;

        if (shadowMapCoords.x >= 0.0f && shadowMapCoords.y >= 0.0f && shadowMapCoords.x <= 0.5f && shadowMapCoords.y <= 0.5f)
        {
            found = true;
        }
    
        if (!found)
        {
            shadowMapCoords = mul(_PosWorld, gCascadedShadowTransform[1]).xyz;

            if (shadowMapCoords.x >= 0.5f && shadowMapCoords.y >= 0.0f && shadowMapCoords.x <= 1.0f && shadowMapCoords.y <= 0.5f)
            {
                found = true;
            }
        }
        if (!found)
        {
            shadowMapCoords = mul(_PosWorld, gCascadedShadowTransform[2]).xyz;

            if (shadowMapCoords.x >= 0.0f && shadowMapCoords.y >= 0.5f && shadowMapCoords.x <= 0.5f && shadowMapCoords.y <= 1.0f)
            {
                found = true;
            }
        }
        if (!found)
        {
            shadowMapCoords = mul(_PosWorld, gCascadedShadowTransform[3]).xyz;

            if (shadowMapCoords.x >= 0.5f && shadowMapCoords.y >= 0.5f && shadowMapCoords.x <= 1.0f && shadowMapCoords.y <= 1.0f)
            {
                found = true;
            }
        }

        float shadowMapSample = tShadowMap_SunLight.Sample(sampler_point_borderBlack, shadowMapCoords.xy);
        float shadowFactor = (float) (shadowMapSample > shadowMapCoords.z);
    
        
        _LitColor += _Lit * shadowFactor.xxx;
    }


    for (uint _SLightIndex = 0; _SLightIndex < gSLightCount; ++_SLightIndex)
    {
        float3 _Lit = ComputeSpotLight(gSpotLights[_SLightIndex], _Color.xyz, _SpecularFactors.w, _SpecularFactors.xyz, _PosWorld.xyz, _NormalWorld, _ToEye);
		
        float4 shadowMapCoords = mul(_PosWorld, gSpotLights[_SLightIndex].ShadowTransform);
        shadowMapCoords.xyz /= shadowMapCoords.w;
				
        float shadowFactor = 0.0f;
		[unroll]
        for (int i = -2; i < 2; ++i)
        {
			[unroll]
            for (int j = -2; j <= 2; ++j)
            {
                float shadowMapSample = tShadowMap_SpotLight.Sample(sampler_point_borderWhite, shadowMapCoords.xy, int2(i, j));
                shadowFactor += (float) (shadowMapSample > shadowMapCoords.z);
            }
        }
    
        shadowFactor *= 0.04f;
        
        //float _ShadowFactor = tShadowMap_SpotLight.SampleCmp(samplerComparison_less_point_borderWhite, shadowMapCoords.xy, shadowMapCoords.z);

        _LitColor += _Lit * shadowFactor.xxx;
    }
    
//Add Global Ambient Light Factor
    _LitColor += (_Color.xyz * gAmbientLightStrength);
    
    return float4(_LitColor, 1.0f);

    

}