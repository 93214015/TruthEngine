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

const float3 F0 = float3(.4f, .4f, .4f);

/////////////////////////////////////////////////////////////////

float DistributionGGX(float3 _Normal, float3 _HalfVector, float _Roughness)
{
}
float GeometrySchlickGGX(float _NdotV, float _Roughness)
{
}
float GeometrySmith(float3 _Normal, float3 _View, float3 _LightVector, float _Roughness)
{
}
float3 FresnelSchlick(float3 F0, float CosTheta)
{
}



///////////////////////////////////////////////////////////////////
//////////////// Helper Funcitions
///////////////////////////////////////////////////////////////////

float ClacAttenuation(float d, float fallOffStart, float fallOffEnd)
{
    return saturate((fallOffEnd - d) / (fallOffEnd - fallOffStart));
}

//Schlik gives an approximation to fresnel reflectance
//float3 SchlikFresnel(float3 r0, float3 normal, float3 lightVector)
//{
//    float cosIncidentAngle = saturate(dot(normal, lightVector));
//    float f0 = 1.0f - cosIncidentAngle;
//    float3 reflectPercent = r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);
//    return reflectPercent;
//}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, float3 _MaterialAlbedo, float _MaterialShininess, float3 _MaterialFrenselR0)
{

    //Derive m from shininess which is derived from roughness.    
    const float m = _MaterialShininess * 256.0f;
    //const float m = mat.Shininess;
    float3 halfVec = normalize(toEye + lightVec);
    
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    roughnessFactor = max(roughnessFactor, 0.0f);
    
    float3 fresnelFactor = SchlikFresnel(_MaterialFrenselR0, normal, lightVec);
    
    //our Specular formula goes outside [0,1] but we are doing LDR rendering. so scale it down a bit
    float3 specAlbedo = fresnelFactor * roughnessFactor;
    
	//specAlbedo = specAlbedo / (specAlbedo + 1.0f);
    
    return (_MaterialAlbedo + specAlbedo) * lightStrength;

}

float CalculateAttenuation(float _Distance, float _FalloffStart, float _FalloffEnd)
{
    //Linear Falloff
    return saturate((_FalloffEnd - _Distance) / (_FalloffEnd - _FalloffStart));
}

float3 ComputeDirectLight(CLightDirectionalData _Light, float3 _MaterialAlbedo, float _MaterialRoughness, float _MaterialMetallic, float3 _Position, float3 _Normal, float3 _View)
{
    float3 _LightVector = normalize(_Light.Direction) * -1.0f;
    
    float _LdotN = dot(_LightVector, _Normal);

    _LdotN = max(_LdotN, 0.0f);
    
    float3 _Radiance = _LdotN * _Light.Strength.xyz;
    
    float3 _HalfVector = normalize(_View + _LightVector);
    
    float NDF = DistributionGGX(_Normal, _HalfVector, _MaterialRoughness);
    float G = GeometrySmith(_Normal, _View, _LightVector, _MaterialRoughness);
    float F = FresnelSchlick()
    
    return BlinnPhong(_LightStrength, _LightVector, _Normal, _ToEye, _MaterialAlbedo, _MaterialShininess, _MaterialFrensel);
}

float3 ComputeSpotLight(CLightSpotData _Light, float3 _MaterialAlbedo, float _MaterialShininess, float3 _MaterialFrensel, float3 _Position, float3 _Normal, float3 _ToEye)
{
    //Distance from surface to light
    float3 _LightPosition = _Light.Position;
    float3 _LightVector = _LightPosition - _Position;
    float _Distance = length(_LightVector);
    
    if (_Distance > _Light.FalloffEnd)
        return 0.0f;
	
    _LightVector /= _Distance;
	
    float _LightFactor = dot(_LightVector, _Normal);
	
    _LightFactor = max(_LightFactor, 0.0f);
	
    float3 _LightStrength = _LightFactor * _Light.Strength.xyz;
	
    float _DistanceAttenuation = CalculateAttenuation(_Distance, _Light.FalloffStart, _Light.FalloffEnd);
    _LightStrength *= _DistanceAttenuation;
	
    float _CosAngle = dot(-1.0 * _LightVector, _Light.Direction);
    float _CosAttenuration = saturate((_CosAngle - _Light.SpotOuterConeCos) * (_Light.SpotOuterConeAngleRangeCosRcp));
    _CosAttenuration *= _CosAttenuration;
	
    _LightStrength *= _CosAttenuration;

    return BlinnPhong(_LightStrength, _LightVector, _Normal, _ToEye, _MaterialAlbedo, _MaterialShininess, _MaterialFrensel);
}

/////////////////////////////////////////////////////////////////