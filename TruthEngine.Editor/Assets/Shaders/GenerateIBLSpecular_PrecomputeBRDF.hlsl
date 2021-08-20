#include "MathHelper.hlsli"


///////////////////////////////////////////////////
//////////////// Vertex Shader
///////////////////////////////////////////////////

const static float4 Vertecies[4] =
{
    float4(-1.0f, 1.0f, 1.0f, 1.0f),
    float4(1.0f, 1.0f, 1.0f, 1.0f),
    float4(-1.0f, -1.0f, 1.0f, 1.0f),
    float4(1.0f, -1.0f, 1.0f, 1.0f),
};

struct VSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
};

VSOut vs(uint _VertexID : SV_VertexID)
{
    VSOut _VSOut;
    _VSOut.Position = Vertecies[_VertexID];
    
    _VSOut.UV = (_VSOut.Position.xy * float2(0.5f, -0.5f)) + float2(0.5f, 0.5);
    
    return _VSOut;
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

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);
	
    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}


float2 IntegrateBRDF(float NdotV, float roughness)
{
    float3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    float3 N = float3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, N, roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return float2(A, B);
}

///////////////////////////////////////////////////
//////////////// Pixel Shader
///////////////////////////////////////////////////

float4 ps(VSOut _VSOut) : SV_Target
{
    float2 integratedBRDF = IntegrateBRDF(_VSOut.UV.x, _VSOut.UV.y);
    return float4(integratedBRDF, 0.0f, 0.0f);

}