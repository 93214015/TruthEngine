#ifndef _LIGHTSPBR
#define _LIGHTSPBR

#include "MathHelper.hlsli"


/////////////////////////////////////////////////////////////////
//////////////// Data Structs
/////////////////////////////////////////////////////////////////

struct CLightDirectionalData
{
    float3 Strength;
    float LightSize;

    float3 Direction;
    bool CastShadow;
    
    float3 Position;
    float Pad0;
    //row_major matrix shadowTransform;
};

struct CLightSpotData
{
    row_major matrix ShadowTransform;
	
    float3 Strength;
    float LightSize;

    float3 Direction;
    bool CastShadow;
	
    float3 Position;
    float FalloffStart;


    float FalloffEnd;
    float SpotOuterConeCos;
    float SpotOuterConeAngleRangeCosRcp;
    float pad;
};

/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////

float DistributionGGX(float3 _Normal, float3 _HalfVector, float _Roughness)
{
    float a = _Roughness * _Roughness;
    float a2 = a * a;
    float NdotH = max(dot(_Normal, _HalfVector), 0.0f);
    float NdotH2 = NdotH * NdotH;
    
    float _Numerator = a2;
    float _Denominator = (NdotH2 * (a2 - 1.0f) + 1.0f);
    _Denominator = PI * _Denominator * _Denominator;
    
    return _Numerator / _Denominator;
}
float GeometrySchlickGGX(float _NdotV, float _Roughness)
{
    float r = _Roughness + 1.0f;
    float k = (r * r) / 8.0f;
    
    float _Numerator = _NdotV;
    float _Denominator = _NdotV * (1.0f - k) + k;

    return _Numerator / _Denominator;
}
float GeometrySmith(float3 _Normal, float3 _View, float3 _LightVector, float _Roughness)
{
    float _NdotV = max(dot(_Normal, _View), 0.0f);
    float ggx1 = GeometrySchlickGGX(_NdotV, _Roughness);
    
    float _NdotL = max(dot(_Normal, _LightVector), 0.0f);
    float ggx2 = GeometrySchlickGGX(_NdotL, _Roughness);
    
    return ggx1 * ggx2;
}
float3 FresnelSchlick(float3 F0, float CosTheta)
{
    return F0 + (1.0f - F0) * pow(max(1.0f - CosTheta, 0.0f), 5.0f);
}
float3 FresnelSchlickRoughness(float3 _F0, float _CosTheta, float _Roughness)
{
    return _F0 + (max((1.0f - _Roughness).xxx, _F0) - _F0) * pow(max(1.0f - _CosTheta, 0.0f), 5.0f);

}



///////////////////////////////////////////////////////////////////
//////////////// Helper Funcitions
///////////////////////////////////////////////////////////////////


float CalculateAttenuation(float _Distance, float _FalloffStart, float _FalloffEnd)
{
    //Linear Falloff
    return saturate((_FalloffEnd - _Distance) / (_FalloffEnd - _FalloffStart));
}

float3 ComputeDirectLight(CLightDirectionalData _Light, float3 _MaterialAlbedo, float _MaterialRoughness, float _MaterialMetallic, float3 F0, float3 _Normal, float3 _View)
{
    float3 _LightVector = normalize(_Light.Direction) * -1.0f;
        
    float3 _HalfVector = normalize(_View + _LightVector);
    
    float _NDF = DistributionGGX(_Normal, _HalfVector, _MaterialRoughness);
    float _G = GeometrySmith(_Normal, _View, _LightVector, _MaterialRoughness);
    float3 _F = FresnelSchlick(F0, max(0.0f, dot(_HalfVector, _View)));
    
    float3 _Numerator = _NDF * _G * _F;
    float _Denominator = 4.0f * max(0.0f, dot(_Normal, _View)) * max(0.0f, dot(_Normal, _LightVector));
    float3 _Specular = _Numerator / max(_Denominator, 0.001);

    float _NdotL = max(0.0f, dot(_LightVector, _Normal));
    
    float3 _Ks = _F;
    float3 _Kd = float(1.0f).xxx - _Ks;
    _Kd *= 1.0f - _MaterialMetallic;
    
    return ((_Kd * _MaterialAlbedo / PI) + _Specular) * _Light.Strength * _NdotL;
}

float3 ComputeSpotLight(CLightSpotData _Light, float3 _MaterialAlbedo, float _MaterialRoughness, float _MaterialMetallic, float3 F0, float3 _Position, float3 _Normal, float3 _View)
{
    //Distance from surface to light
    float3 _LightPosition = _Light.Position;
    float3 _LightVector = _LightPosition - _Position;
    float _Distance = length(_LightVector);
    
    if (_Distance > _Light.FalloffEnd)
        return 0.0f;
	
    _LightVector /= _Distance;
        
    float3 _HalfVector = normalize(_View + _LightVector);
    
    float _NDF = DistributionGGX(_Normal, _HalfVector, _MaterialRoughness);
    float _G = GeometrySmith(_Normal, _View, _LightVector, _MaterialRoughness);
    float3 _F = FresnelSchlick(F0, max(0.0f, dot(_HalfVector, _View)));
    
    float3 _Numerator = _NDF * _G * _F;
    float _Denominator = 4.0f * max(0.0f, dot(_Normal, _View)) * max(0.0f, dot(_Normal, _LightVector));
    float3 _Specular = _Numerator / max(_Denominator, 0.001);

    float _NdotL = max(0.0f, dot(_LightVector, _Normal));
    
    float3 _Ks = _F;
    float3 _Kd = float3(1.0f, 1.0f, 1.0f) - _Ks;
    _Kd *= 1.0f - _MaterialMetallic;
    
    float3 _Radiance = _Light.Strength * CalculateAttenuation(_Distance, _Light.FalloffStart, _Light.FalloffEnd).xxx;
    
    return ((_Kd * _MaterialAlbedo / PI) + _Specular) * _Radiance * _NdotL;
}

/////////////////////////////////////////////////////////////////


#endif