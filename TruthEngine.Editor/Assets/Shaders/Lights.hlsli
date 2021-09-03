#ifndef _LIGHTS
#define _LIGHTS


/////////////////////////////////////////////////////////////////
//////////////// Data Structs
/////////////////////////////////////////////////////////////////

struct CLightDirectionalData
{
    float3 Position;
    float LightSize;

    float3 Strength;
    bool CastShadow;
        
    float3 Direction;
    float Pad0;
    //row_major matrix shadowTransform;
};

struct CLightSpotData
{
    row_major matrix ShadowTransform;

    float3 Position;
    float LightSize;
	
    float3 Strength;
    bool CastShadow;
    	
    float3 Direction;
    float FalloffStart;


    float FalloffEnd;
    float SpotOuterConeCos;
    float SpotOuterConeAngleRangeCosRcp;
    float pad;
};

struct CLightPointData
{
    float3 Position;
    float LightSize;
    
    float3 Strength;
    bool CastShadow;
    
    float RadiusAttenuationStart;
    float RadiusAttenuationEnd;
    float2 pad;
    
    //float AttenuationConstant;
    //float AttenuationLinear;
    //float AttenuationQuadratic;
    //float pad;
};

/////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//////////////// Helper Funcitions
///////////////////////////////////////////////////////////////////


//Schlik gives an approximation to fresnel reflectance
float3 SchlikFresnel(float3 r0, float3 normal, float3 lightVector)
{
    float cosIncidentAngle = saturate(dot(normal, lightVector));
    
    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);
    return reflectPercent;
}

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

float CalculateAttenuationSpotLight(float _Distance, float _FalloffStart, float _FalloffEnd)
{
    //Quadrant Falloff
    float _Falloff = saturate((_FalloffEnd - _Distance) / (_FalloffEnd - _FalloffStart));
    _Falloff *= _Falloff;
    return _Falloff;
}

float3 ComputeDirectLight(CLightDirectionalData _Light, float3 _MaterialAlbedo, float _MaterialShininess, float3 _MaterialFrensel, float3 _Position, float3 _Normal, float3 _ToEye)
{
    float3 _LightVector = -1.0 * normalize(_Light.Direction);
    float _LightFactor = dot(_LightVector, _Normal);
    _LightFactor = max(_LightFactor, 0.0f);
    
    float3 _LightStrength = _LightFactor * _Light.Strength.xyz;
    
    return BlinnPhong(_LightStrength, _LightVector, _Normal, _ToEye, _MaterialAlbedo, _MaterialShininess, _MaterialFrensel);
}

float3 ComputeSpotLight(CLightSpotData _Light, float3 _MaterialAlbedo, float _MaterialShininess, float3 _MaterialFrensel, float3 _Position, float3 _Normal, float3 _ToEye)
{
    //Distance from surface to light
    float3 _LightPosition = _Light.Position;
    float3 _LightVector = _LightPosition - _Position;
    float _Distance = length(_LightVector);
    _LightVector /= _Distance;
    
    float _DistanceAttenuation = CalculateAttenuationSpotLight(_Distance, _Light.FalloffStart, _Light.FalloffEnd);
    
    float _CosAngle = dot(-1.0 * _LightVector, _Light.Direction);
    float _CosAttenuration = saturate((_CosAngle - _Light.SpotOuterConeCos) * (_Light.SpotOuterConeAngleRangeCosRcp));
    _CosAttenuration *= _CosAttenuration;
    
    float _Attenuation = _DistanceAttenuation * _CosAttenuration;

    if (_Distance > _Light.FalloffEnd || _Attenuation < 0.001)
        return 0.0f;
	
	
    float _LightFactor = max(dot(_LightVector, _Normal), 0.0f);
	
    float3 _LightStrength = _LightFactor * _Light.Strength.xyz;
	
	
    _LightStrength *= _Attenuation.xxx;

    return BlinnPhong(_LightStrength, _LightVector, _Normal, _ToEye, _MaterialAlbedo, _MaterialShininess, _MaterialFrensel);
}

float3 ComputePointLight(CLightPointData _Light, float3 _MaterialAlbedo, float _MaterialShininess, float3 _MaterialFresnel, float3 _Position, float3 _Normal, float3 _ToEye)
{
    float3 _LightVector = _Light.Position - _Position;
    float _Distance = length(_LightVector);
    
    //float _Attenuation = 1.0f / (_Light.AttenuationConstant + (_Light.AttenuationLinear * _Distance) + (_Light.AttenuationQuadratic * _Distance * _Distance) );
    
    if(_Distance > _Light.RadiusAttenuationEnd)
        return 0.0f;
    
    _LightVector /= _Distance;
    
    float _LightFactor = max(dot(_Normal, _LightVector), 0.0f);
    
    float _Attenuation = saturate((_Light.RadiusAttenuationEnd - _Distance) / (_Light.RadiusAttenuationEnd - _Light.RadiusAttenuationStart));
    _Attenuation *= _Attenuation;
    float3 _Radiance = _Light.Strength.xyz * (_LightFactor * _Attenuation);
    
    return BlinnPhong(_Radiance, _LightVector, _Normal, _ToEye, _MaterialAlbedo, _MaterialShininess, _MaterialFresnel);
    
}

/////////////////////////////////////////////////////////////////

#endif