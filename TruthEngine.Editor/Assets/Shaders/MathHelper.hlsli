
float ConvertToLinearDepth(float _Depth, float _Proj33, float _Proj43)
{
    return _Proj43 / (_Depth - _Proj33);
}

float4 ReconstructWorldPosition(float2 _PosCS, float _DepthView, float4 _PerspectiveValues, row_major matrix _ViewInverse)
{
    
}