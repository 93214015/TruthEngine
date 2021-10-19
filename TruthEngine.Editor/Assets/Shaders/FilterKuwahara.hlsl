
#ifdef __INTELLISENSE__
#define SIZE 5
#define TEX_TYPE float
#endif

#define ROW_SIZE (SIZE * 2 + 1)
#define KERNEL_SIZE (ROW_SIZE * ROW_SIZE)

Texture2D<TEX_TYPE> gTexInput : register(t0);

/////////////////////////////////////////////////////////////////
//////////////// Vertex Shader
/////////////////////////////////////////////////////////////////

//Defined For TriangleStrip Topology
static const float2 VertexPositions[4] =
{
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

struct VertexOut
{
	float4 PosH : SV_Position;
	float2 UV : TEXCOORD0;
};

VertexOut vs(uint vertexID : SV_VertexID)
{
	VertexOut _VOut;
	_VOut.PosH = float4(VertexPositions[vertexID], 0.0f, 1.0f);
	_VOut.UV = VertexPositions[vertexID] * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	return _VOut;
}

/////////////////////////////////////////////////////////////////
//////////////// Pixel Shader
/////////////////////////////////////////////////////////////////


#if (TEX_TYPE != float)
#define ColorRatio float3(0.3, 0.59, 0.11)
#define GreyScaleColor(color) dot(color.xyz, ColorRatio)  
#define RETURN_COLOR(color) float4(color.xyz, 1.0f)
#else
#define GreyScaleColor(color) color 
#define RETURN_COLOR(color) float4(color, 0.0f, 0.0f, 0.0f)
#endif


void FindMean(inout TEX_TYPE _Mean, inout float _MinVariance, int2 uv, int i0, int i1, int j0, int j1)
{
	TEX_TYPE _MeanTemp = 0.0f;
	uint _Count = 0;
	float _Values[(SIZE + 1)*(SIZE + 1)];

	for (int i = i0; i <= i1; ++i)
	{
		for (int j = j0; j <= j1; ++j)
		{
			TEX_TYPE _Color = gTexInput.Load(int3(uv.x + i, uv.y + j, 0));

			_MeanTemp += _Color;

			_Values[_Count] = GreyScaleColor(_Color);

			_Count++;
		}
	}

	_MeanTemp /= _Count;

	const float _ValueMean = GreyScaleColor(_MeanTemp);

	float _Variance = 0.0;

	for (uint i = 0; i < (SIZE + 1) * (SIZE + 1); ++i)
	{
		_Variance += pow(_Values[i] - _ValueMean, 2);
	}

	_Variance /= _Count;


	if (_Variance < _MinVariance || _MinVariance < 0.0f)
	{
		_Mean = _MeanTemp;
		_MinVariance = _Variance;
	}

}

float4 ps(VertexOut _vout) : SV_TARGET
{
	TEX_TYPE _Mean = 0.0f;
	float _MinVariance = -1.0f;

	const int2 uv = int2(_vout.PosH.x, _vout.PosH.y);

	//Upper Left
	FindMean(_Mean, _MinVariance, uv, -SIZE, 0, -SIZE, 0);

	//Upper Right
	FindMean(_Mean, _MinVariance, uv, 0, SIZE, -SIZE, 0);

	//Lower Left
	FindMean(_Mean, _MinVariance, uv, -SIZE, 0, 0, SIZE);

	//Lower Right
	FindMean(_Mean, _MinVariance, uv, 0, SIZE, 0, SIZE);

	return RETURN_COLOR(_Mean);
}