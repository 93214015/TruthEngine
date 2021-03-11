

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// HDR Average Lumination First Pass - Average Lumination of the scene
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cbuffer CBDownScaleConstants : register(b0)
{
    uint2 gScreenQuarterSizeResolution : packoffset(c0.x); //Resolution of Quarter Size of HDR Source Texture
    uint gDomain : packoffset(c0.z); // Total Pixel in down-scaled image
    uint gGroupSize : packoffset(c0.w); // Groups number dispatched on first pass
}

RWStructuredBuffer<float> AverageLum : register(u0);

Texture2D tHDR : register(t0);



groupshared float gSharedPosition[1024];

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

float DownScale4X4(uint2 _CurrPixel, uint _GroupThreadID)
{
    float _AvgLum = 0.0f;
    
    if (_CurrPixel.y < gScreenQuarterSizeResolution.y)
    {
        int3 _FullResPos = int3(_CurrPixel * 4, 0);
        float4 _AvgValue = float4(.0f, .0f, .0f, .0f);

        [unroll]
        for (uint i = 0; i < 4; ++i)
        {
            [unroll]
            for (uint j = 0; j < 4; ++j)
            {
                _AvgValue += tHDR.Load(_FullResPos, int2(i, j));
            }
        }
        
        _AvgValue *= 0.0625f; // 1/16

        _AvgLum = dot(LUM_FACTOR, _AvgValue);

    }
    
    gSharedPosition[_GroupThreadID] = _AvgLum;
    
    GroupMemoryBarrierWithGroupSync();
    
    return _AvgLum;
}

float DownScale1024To4(uint _DispatchThreadID, uint _GroupThreadID, float _AvgLum)
{
    
    for (uint _step1 = 1, _step2 = 2, _step3 = 3, _kernelSize = 4; _kernelSize < 1024; _step1 *= 4, _step2 *= 4, _step3 *= 4, _kernelSize *= 4)
    {
        if (_GroupThreadID % _kernelSize == 0)
        {
            float _AvgValue = _AvgLum;
            _AvgValue += (_DispatchThreadID + _step1) < gDomain ? gSharedPosition[_GroupThreadID + _step1] : _AvgLum;
            _AvgValue += (_DispatchThreadID + _step2) < gDomain ? gSharedPosition[_GroupThreadID + _step2] : _AvgLum;
            _AvgValue += (_DispatchThreadID + _step3) < gDomain ? gSharedPosition[_GroupThreadID + _step3] : _AvgLum;

            _AvgLum = _AvgValue;
            gSharedPosition[_GroupThreadID] = _AvgValue;
        }
        
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    return _AvgLum;
}

void DownScale4To1(uint _DispatchThreadID, uint _GroupThreadID, uint _GroupID, float _AvgLum)
{
    if(_GroupThreadID == 0)
    {
        float _StepAvgLum = _AvgLum;
        _StepAvgLum += (_DispatchThreadID + 256) < gDomain ? gSharedPosition[256] : _AvgLum;
        _StepAvgLum += (_DispatchThreadID + 512) < gDomain ? gSharedPosition[512] : _AvgLum;
        _StepAvgLum += (_DispatchThreadID + 768) < gDomain ? gSharedPosition[768] : _AvgLum;
        
        _StepAvgLum *= 0.0009765625f; // 1/1024
        
        AverageLum[_GroupID] = _StepAvgLum;
    }
}

[numthreads(1024, 1, 1)]
void HDRDownScalingFirstPass(uint3 _DispatchThreadID : SV_DispatchThreadID, uint3 _GroupThreadID : SV_GroupThreadID, uint3 _GroupID : SV_GroupID)
{
    uint2 _CurrPixel = uint2(_DispatchThreadID.x % gScreenQuarterSizeResolution.x, _DispatchThreadID.x / gScreenQuarterSizeResolution.x);
    
    float _AvgLum = DownScale4X4(_CurrPixel, _GroupThreadID.x);
    
    _AvgLum = DownScale1024To4(_DispatchThreadID.x, _GroupThreadID.x, _AvgLum);
    
    DownScale4To1(_DispatchThreadID.x, _GroupThreadID.x, _GroupID.x, _AvgLum);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// HDR Average Lumination Second Pass - Convert 1D Average Values of first pass to single value
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_GROUPS 64

StructuredBuffer<float> AverageValues1D : register(t0);

groupshared float gSharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void HDRDownScalingSecondPass(uint3 _DispatchThreadID : SV_DispatchThreadID, uint3 _GroupThreadID : SV_GroupThreadID, uint3 _GroupID : SV_GroupID, uint _GroupIndex : SV_GroupIndex)
{
    float _AvgLum = .0f;
    if (_DispatchThreadID.x < gGroupSize)
    {
        _AvgLum = AverageValues1D[_DispatchThreadID.x];
    }
    gSharedAvgFinal[_DispatchThreadID.x] = _AvgLum;
    
    GroupMemoryBarrierWithGroupSync();
    
    
    //DownScale 64 to 16
    if (_DispatchThreadID.x % 4 == 0)
    {
        float _StepAvgLum = _AvgLum;

        uint _Index = _DispatchThreadID.x + 1;
        _StepAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        _Index = _DispatchThreadID.x + 2;
        _StepAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        _Index = _DispatchThreadID.x + 3;
        _StepAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;

        _AvgLum = _StepAvgLum;
        gSharedAvgFinal[_DispatchThreadID.x] = _StepAvgLum;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    //DownScale 16 to 4
    if (_DispatchThreadID.x % 16 == 0)
    {
        float _StepAvgLum = _AvgLum;
        uint _Index = _DispatchThreadID.x + 4;
        _StepAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        _Index = _DispatchThreadID.x + 8;
        _StepAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        _Index = _DispatchThreadID.x + 12;
        _StepAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;

        _AvgLum = _StepAvgLum;
        gSharedAvgFinal[_DispatchThreadID.x] = _StepAvgLum;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    //DownScale 4 to 1
    if (_DispatchThreadID.x == 0)
    {
        float _FinalAvgLum = _AvgLum;
        uint _Index = _DispatchThreadID.x + 16;
        _FinalAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        _Index = _DispatchThreadID.x + 32;
        _FinalAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        _Index = _DispatchThreadID.x + 48;
        _FinalAvgLum += _Index < gGroupSize ? gSharedAvgFinal[_Index] : _AvgLum;
        
        
        _FinalAvgLum *= 0.015625; // _StepAvgLum / 64.0f
        gSharedAvgFinal[_DispatchThreadID.x] = _FinalAvgLum;
        
        //Store Final Value
        AverageLum[0] = max(_FinalAvgLum, .0001f);

    }
    
}
