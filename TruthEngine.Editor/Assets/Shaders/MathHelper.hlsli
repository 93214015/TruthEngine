
const float PI = 3.14159265359;

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