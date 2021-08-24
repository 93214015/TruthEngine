#include "LightsPBR.hlsli"
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
Texture2D<float> tShadowMap_SunLight : register(t7);
Texture2D<float> tShadowMap_SpotLight : register(t8);


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

    if (_DepthProj > 0.9999f)
        discard;
    
    float _DepthView = ConvertToLinearDepth(_DepthProj, ProjectionValues.z, ProjectionValues.w);
    
    float4 _PosWorld = ReconstructWorldPosition(_VOut.PosCS, _DepthView, ProjectionValues, ViewInverse);
    
    float3 _NormalWorld = tNormal.Sample(sampler_point_wrap, _VOut.UV) * 2.0f - 1.0f;

    float3 _Albedo = tColor.Sample(sampler_point_wrap, _VOut.UV).xyz;

    float4 _SpecularFactors = tSpecular.Sample(sampler_point_wrap, _VOut.UV); // SpecularFctors.x = Roughness ; SpecularFctors.y = Metallic ; SpecularFctors.z = AmbientOcclusion
      
    
        
    float3 _ToEye = normalize(EyePos.xyz - _PosWorld.xyz);

    float3 _LitColor = float3(.0f, .0f, .0f);

    float3 _F0 = float3(0.04f, 0.04f, 0.04f);
    _F0 = lerp(_F0, _Albedo, _SpecularFactors.y);
    
    for (uint _DLightIndex = 0; _DLightIndex < gDLightCount; ++_DLightIndex)
    {
        
        float3 _Lit = ComputeDirectLight(gDLights[_DLightIndex], _Albedo.xyz, _SpecularFactors.x, _SpecularFactors.y, _F0, _NormalWorld, _ToEye);
		
        //code for cascaded shadow map; finding corrsponding shadow map cascade and coords

        /***

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

        ***/
        
        _LitColor += _Lit;
        
    }


    for (uint _SLightIndex = 0; _SLightIndex < gSLightCount; ++_SLightIndex)
    {
        float3 _Lit = ComputeSpotLight(gSpotLights[_SLightIndex], _Albedo, _SpecularFactors.x, _SpecularFactors.y, _F0, _PosWorld.xyz, _NormalWorld, _ToEye);
		
        
        /*
        
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

        */        
        
        _LitColor += _Lit;

    }
    
    float _NdotV = max(dot(_NormalWorld, _ToEye), 0.0f);
    
    float3 _Ks = FresnelSchlickRoughness(_F0, _NdotV, _SpecularFactors.x);
    float3 _Kd = 1.0 - _Ks;
    _Kd *= 1.0f - _SpecularFactors.y;
    
    float3 _Irrediance = tIBLAmbient.Sample(sampler_linear, _NormalWorld).rgb;
    float3 _Diffuse = _Irrediance * _Albedo;

    float3 _ReflectVector = reflect(-_ToEye, _NormalWorld);
    const float MAX_REFLECTION_LOD = 4.0f;
    float3 _PrefilteredIBLSpecular = tIBLSpecular.SampleLevel(sampler_linear, _ReflectVector, _SpecularFactors.x * MAX_REFLECTION_LOD).rgb;
    float2 _PrecomputeBRDF = tPrecomputedBRDF.Sample(sampler_linear, float2(_NdotV, _SpecularFactors.x)).rg;
    float3 _Specular = _PrefilteredIBLSpecular * (_Ks * _PrecomputeBRDF.x + _PrecomputeBRDF.y);

    float3 _Ambient = (_Kd * _Diffuse + _Specular) * _SpecularFactors.z;
	
	//Add Global Ambient Light Factor
    //litColor += (_MaterialAlbedo * gAmbientLightStrength * _AmbientOcclusion.xxx);
    _LitColor += _Ambient;
    
    
    //Add Global Ambient Light Factor
    //_LitColor += (_Albedo.xyz * gAmbientLightStrength);
    
#ifndef ENABLE_HDR
    _LitColor /= _LitColor + float3(1.0f, 1.0f, 1.0f); //clamp values for LDR lighting
    _LitColor = pow(_LitColor, (1.0f / 2.2f).xxx);
#endif
    
    return float4(_LitColor, 1.0f);

    

}