
#ifdef __INTELLISENSE__
#define TEX_TYPE float
#endif

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

#define SIZE 5
#define ROW_SIZE (SIZE * 2 + 1)
#define KERNEL_SIZE (ROW_SIZE * ROW_SIZE)
#define MAX_BIN_SIZE 100
#define MEDIAN_LIMIT (KERNEL_SIZE / 2) + 1

float4 ps(VertexOut _vout) : SV_Target
{
	uint _bins[MAX_BIN_SIZE];
	uint _binsIndex[KERNEL_SIZE];

	for (uint i = 0; i < MAX_BIN_SIZE; i++)
	{
		_bins[i] = 0;
	}

	#if (TEX_TYPE != float)
		const float3 _GreyScaleRatio = float3(0.3, 0.59, 0.11);
	#endif

	int2 _PixelCoord = int2((int)_vout.PosH.x, (int)_vout.PosH.y);

	TEX_TYPE _Colors[KERNEL_SIZE];
	uint _Count = 0;

	for (int i = -SIZE; i <= SIZE; ++i)
	{
		for (int j = -SIZE; j <= SIZE; ++j)
		{

			#if (TEX_TYPE == float)
					float _value = gTexInput.Load(int3(_PixelCoord.x + i, _PixelCoord.y + j, 0));
					_Colors[_Count] = _value;
			#else
					float3 _Pixel = gTexInput.Load(int3(_PixelCoord.x + i, _PixelCoord.y + j, 0)).xyz;
					_Colors[_Count].xyz = _Pixel;
					float _value = dot(_Pixel, _GreyScaleRatio);
			#endif

			
			uint _Index = uint(floor(_value * MAX_BIN_SIZE));
			_Index = clamp(_Index, 0, MAX_BIN_SIZE - 1);
			_bins[_Index] += 1;
			_binsIndex[_Count] = _Index;

			_Count++;
		}
	}

	uint _Total = 0;
	uint _bIndex = 0;

	for (uint i = 0; i <= MAX_BIN_SIZE; ++i)
	{
		_Total += _bins[i];

		if (_Total >= MEDIAN_LIMIT)
		{
			_bIndex = i;
			break;
		}
	}

	TEX_TYPE _ResultColor;

	for (uint i = 0; i < KERNEL_SIZE; ++i)
	{
			if (_binsIndex[i] == _bIndex)
			{
				_ResultColor = _Colors[i];
				break;
			}
	}

#if (TEX_TYPE == float)
	return float4(_ResultColor, 0.0f, 0.0f, 1.0f);
#else
	return float4(_ResultColor.xyz, 1.0f);
#endif
}
