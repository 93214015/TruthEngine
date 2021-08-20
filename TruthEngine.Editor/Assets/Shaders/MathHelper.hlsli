#ifndef _MATHHELPER
#define _MATHHELPER


const static float PI = 3.14159265359;

float ConvertToLinearDepth(float _Depth, float _Proj33, float _Proj43)
{
    return _Proj43 / (_Depth - _Proj33);
}

float4 ReconstructWorldPosition(float2 _PosCS, float _DepthView, float4 _PerspectiveValues, row_major matrix _ViewInverse)
{
    float4 _PosView = float4(_PosCS.xy * _PerspectiveValues.xy * _DepthView, _DepthView, 1.0f);

    float4 _PosWorld = mul(_PosView, _ViewInverse);
    
    return _PosWorld;
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
float2 Hammersley(uint i, uint N)
{
    float _idivn = float(i) / float(N);
    return float2(_idivn, RadicalInverse_VdC(i));
}

#endif