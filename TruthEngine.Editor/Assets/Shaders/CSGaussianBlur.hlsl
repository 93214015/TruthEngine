Texture2D<float4> tInput : register(t0);
RWTexture2D<float4> tOutput : register(u0);

cbuffer CB : register(b0)
{
    uint2 gRes : packoffset(c0.x); // Resulotion of the input image: x - width, y - height
    uint2 gPad : packoffset(c0.z);
}

static const float SampleWeights[31] =
{
	0.016131381634610,
	0.017205188393549,
	0.018269097826469,
	0.019312770184099,
	0.020325528464034,
	0.021296533701490,
	0.022214973526120,
	0.023070259545128,
	0.023852228611198,
	0.024551342686888,
	0.025158881846200,
	0.025667124973068,
	0.026069512931697,
	0.026360789392388,
    0.026537115087597,

    0.026596152026762,

	0.026537115087597,
	0.026360789392388,
	0.026069512931697,
	0.025667124973068,
	0.025158881846200,
	0.024551342686888,
	0.023852228611198,
	0.023070259545128,
	0.022214973526120,
	0.021296533701490,
	0.020325528464034,
	0.019312770184099,
	0.018269097826469,
	0.017205188393549,
	0.016131381634610
};

// should pass the kernel size value at the compilation time
#ifdef __INTELLISENSE__ 
#define KernelHalf 15
#endif

#define GroupThreads 64
#define CacheSize (GroupThreads + (2 * KernelHalf))
groupshared float4 gCache[CacheSize];
	

[numthreads(GroupThreads, 1, 1)]
void HorizontalFilter(int3 _DispatchThreadID : SV_DispatchThreadID, int _GroupThreadIndex : SV_GroupIndex)
{
    {
        int coordX = max(_DispatchThreadID.x - KernelHalf, 0);
        int2 coords = int2(coordX, _DispatchThreadID.y);
    
        gCache[_GroupThreadIndex] = tInput.Load(int3(coords, 0));
    }
    
    if (_GroupThreadIndex >= GroupThreads - KernelHalf)
    {
        int _BorderX = gRes.x - 1;
        int coordX = min(_DispatchThreadID.x, _BorderX);
        gCache[_GroupThreadIndex + KernelHalf] = tInput.Load(int3(coordX, _DispatchThreadID.y, 0));
        
        coordX = min(_DispatchThreadID.x + KernelHalf, _BorderX);
        gCache[_GroupThreadIndex + (2 * KernelHalf)] = tInput.Load(int3(coordX, _DispatchThreadID.y, 0));
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 _Result = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    [unroll]
    for (int i = -KernelHalf; i <= KernelHalf; ++i)
    {
        int k = i + KernelHalf;
        
        _Result += gCache[_GroupThreadIndex + k] * SampleWeights[k];
    }
    
    tOutput[_DispatchThreadID.xy] = _Result;
}

[numthreads(1, GroupThreads, 1)]
void VerticalFilter(int3 _DispatchThreadID : SV_DispatchThreadID, int _GroupThreadIndex : SV_GroupIndex)
{
   {
        int coordY = max(_DispatchThreadID.y - KernelHalf, 0);
        int2 coords = int2(_DispatchThreadID.x, coordY);
    
        gCache[_GroupThreadIndex] = tInput.Load(int3(coords, 0));
    }
    
    if (_GroupThreadIndex >= GroupThreads - KernelHalf)
    {
        int _BorderY = gRes.y - 1;
        int coordY = min(_DispatchThreadID.y, _BorderY);
        gCache[_GroupThreadIndex + KernelHalf] = tInput.Load(int3(_DispatchThreadID.x, coordY, 0));
        
        coordY = min(_DispatchThreadID.y + KernelHalf, _BorderY);
        gCache[_GroupThreadIndex + (2 * KernelHalf)] = tInput.Load(int3(_DispatchThreadID.x, coordY, 0));
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 _Result = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    [unroll]
    for (int i = -KernelHalf; i <= KernelHalf; ++i)
    {
        int k = i + KernelHalf;
        
        _Result += gCache[_GroupThreadIndex + k] * SampleWeights[k];
    }
    
    tOutput[_DispatchThreadID.xy] = _Result;
}