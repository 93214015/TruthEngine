Texture2D<float4> tInput : register(t0);
RWTexture2D<float4> tOutput : register(u0);

cbuffer CB : register(b0)
{
    uint2 gRes : packoffset(c0.x); // Resulotion of the input image: x - width, y - height
    uint2 gPad : packoffset(c0.z);
}

static const float SampleWeights[13] =
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

// should pass the kernel size value at the compilation time
#ifdef __INTELLISENSE__ 
#define KernelHalf 6
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