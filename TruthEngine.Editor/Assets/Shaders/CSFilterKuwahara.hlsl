#ifdef __INTELLISENSE__
#define TEX_TYPE float
#define SIZE 2
#endif

#define ROW_SIZE (SIZE * 2 + 1)
#define KERNLE_SIZE (ROW_SIZE * ROW_SIZE)


Texture2D<TEX_TYPE> gTexInput : register(t0);
RWTexture2D<TEX_TYPE> gTexOutput : register(u0);

cbuffer CB_Kuwahara : register(b0)
{
	uint2 gTextureSize : packoffset(c0.x);
	uint2 _pad0 : packoffset(c0.z);
}

groupshared TEX_TYPE Colors[ROW_SIZE + (2 * SIZE)][ROW_SIZE + (2 * SIZE)];
groupshared float Variances[ROW_SIZE + SIZE][ROW_SIZE + SIZE];

#if (TEX_TYPE != float)
#define ColorRatio float3(0.3, 0.59, 0.11)
#define GreyScaleColor(color) dot(color.xyz, ColorRatio)  
#else
#define GreyScaleColor(color) color 
#endif

TEX_TYPE CalcVariance(const uint2 GTid, out float _Variance)
{
	TEX_TYPE _MeanTemp = 0.0f;
	float _Values[(SIZE + 1) * (SIZE + 1)];


	//uint _Count = 0;

	for (int i = 0; i <= SIZE; i++)
	{
		for (int j = 0; j <= SIZE; j++)
		{
			TEX_TYPE _Color = Colors[GTid.y + i][GTid.x + j];

			_MeanTemp += _Color;

			_Values[i * (SIZE + 1) + j] = GreyScaleColor(_Color);

			//_Count++;
		}
	}

	_MeanTemp /= ((SIZE + 1) * (SIZE + 1));

	const float _ValueMean = GreyScaleColor(_MeanTemp);

	_Variance = 0.0;

	for (int i = 0; i < ((SIZE + 1) * (SIZE + 1)); ++i)
	{
		float _var = _Values[i] - _ValueMean;
		_var *= _var;
		_Variance += _var;
	}

	_Variance /= ((SIZE + 1) * (SIZE + 1));

	Variances[GTid.y][GTid.x] = _Variance;

	return _MeanTemp;

}

[numthreads(ROW_SIZE, ROW_SIZE, 1)]
void cs(uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{
	bool _RightEdge = false;
	bool _BottomEdge = false;
	{
		//Populating shared memory
		Colors[GTid.y + SIZE][GTid.x + SIZE] = gTexInput.Load(int3(DTid.x, DTid.y, 0));

		bool _LeftEdge = false;
		if (GTid.x < SIZE)
		{
			_LeftEdge = true;
			Colors[GTid.y + SIZE][GTid.x] = gTexInput.Load(int3(DTid.x - SIZE, DTid.y, 0));
		}

		bool _TopEdge = false;
		if (GTid.y < SIZE)
		{
			_TopEdge = true;
			Colors[GTid.y][GTid.x + SIZE] = gTexInput.Load(int3(DTid.x, DTid.y - SIZE, 0));
		}


		if (GTid.x > SIZE)
		{
			_RightEdge = true;
			Colors[GTid.y + SIZE][GTid.x + (2 * SIZE)] = gTexInput.Load(int3(DTid.x + SIZE, DTid.y, 0));
		}

		if (GTid.y > SIZE)
		{
			_BottomEdge = true;
			Colors[GTid.y + (2 * SIZE)][GTid.x + SIZE] = gTexInput.Load(int3(DTid.x, DTid.y + SIZE, 0));
		}


		if (_LeftEdge && _TopEdge)
		{
			Colors[GTid.y][GTid.x] = gTexInput.Load(int3(DTid.x - SIZE, DTid.y - SIZE, 0));
		}

		if (_LeftEdge && _BottomEdge)
		{
			Colors[GTid.y + (2 * SIZE)][GTid.x] = gTexInput.Load(int3(DTid.x - SIZE, DTid.y + SIZE, 0));
		}

		if (_RightEdge && _TopEdge)
		{
			Colors[GTid.y][GTid.x + (2 * SIZE)] = gTexInput.Load(int3(DTid.x + SIZE, DTid.y - SIZE, 0));
		}

		if (_RightEdge && _BottomEdge)
		{
			Colors[GTid.y + (2 * SIZE)][GTid.x + (2 * SIZE)] = gTexInput.Load(int3(DTid.x + SIZE, DTid.y + SIZE, 0));
		}
	}

	GroupMemoryBarrierWithGroupSync();

	float _MinVariance;

	float4 _MeanTemp;

	_MeanTemp.x = CalcVariance(GTid.xy, _MinVariance);

	if (_RightEdge)
	{
		float _m;
		_MeanTemp.y = CalcVariance(uint2(GTid.x + SIZE, GTid.y), _m);
	}

	if (_BottomEdge)
	{
		float _m;
		_MeanTemp.z = CalcVariance(uint2(GTid.x + SIZE, GTid.y), _m);
	}

	if (_RightEdge && _BottomEdge)
	{
		float _m;
		_MeanTemp.w = CalcVariance(uint2(GTid.x + SIZE, GTid.y + SIZE), _m);
	}

	GroupMemoryBarrierWithGroupSync();

	Colors[GTid.y][GTid.x] = _MeanTemp.x;

	if (_RightEdge)
	{
		Colors[GTid.y][GTid.x + SIZE] = _MeanTemp.y;
	}

	if (_BottomEdge)
	{
		Colors[GTid.y + SIZE][GTid.x] = _MeanTemp.z;
	}

	if (_RightEdge && _BottomEdge)
	{
		Colors[GTid.y + SIZE][GTid.x + SIZE] = _MeanTemp.w;
	}

	GroupMemoryBarrierWithGroupSync();

	if (DTid.x >= gTextureSize.x || DTid.y >= gTextureSize.y)
	{
		return;
	}

	uint _MinRow = GTid.y;
	uint _MinColumn = GTid.x;

	//Right Side

	uint _Row = GTid.y;
	uint _Column = GTid.x + SIZE;
	float _VarianceTemp = Variances[_Row][_Column];

	if (_VarianceTemp < _MinVariance)
	{
		_MinVariance = _VarianceTemp;
		_MinRow = _Row;
		_MinColumn = _Column;
	}

	//Bottom Side

	_Row = GTid.y + SIZE;
	_Column = GTid.x;
	_VarianceTemp = Variances[_Row][_Column];

	if (_VarianceTemp < _MinVariance)
	{
		_MinVariance = _VarianceTemp;
		_MinRow = _Row;
		_MinColumn = _Column;
	}

	//Right-Bottom Side

	_Row = GTid.y + SIZE;
	_Column = GTid.x + SIZE;
	_VarianceTemp = Variances[_Row][_Column];

	if (_VarianceTemp < _MinVariance)
	{
		_MinVariance = _VarianceTemp;
		_MinRow = _Row;
		_MinColumn = _Column;
	}

	gTexOutput[DTid.xy] = Colors[_MinRow][_MinColumn];

}








TEX_TYPE CalcVariance2(const uint2 GTid, out float _Variance)
{
	if (GTid.x < SIZE || GTid.y < SIZE)
		return -1.0f;

	TEX_TYPE _MeanTemp = 0.0f;
	float _Values[(SIZE + 1) * (SIZE + 1)];


	uint _Count = 0;

	for (int i = -SIZE; i <= 0; i++)
	{
		for (int j = -SIZE; j <= 0; j++)
		{
			TEX_TYPE _Color = Colors[GTid.y + i][GTid.x + j];

			_MeanTemp += _Color;

			_Values[_Count] = GreyScaleColor(_Color);

			_Count++;
		}
	}

	_MeanTemp /= ((SIZE + 1) * (SIZE + 1));

	const float _ValueMean = GreyScaleColor(_MeanTemp);

	_Variance = 0.0;

	for (int i = 0; i < ((SIZE + 1) * (SIZE + 1)); ++i)
	{
		float _var = _Values[i] - _ValueMean;
		_var *= _var;
		_Variance += _var;
	}

	_Variance /= ((SIZE + 1) * (SIZE + 1));

	Variances[GTid.y - SIZE][GTid.x - SIZE] = _Variance;

	return _MeanTemp;

}


[numthreads(ROW_SIZE + (2 * SIZE), ROW_SIZE + (2 * SIZE), 1)]
void cs2(uint3 GTid : SV_GroupThreadID, uint3 GroupID : SV_GroupID)
{

	uint3 _Pixel = uint3(GroupID.x * ROW_SIZE + GTid.x - SIZE, GroupID.y * ROW_SIZE + GTid.y - SIZE, 0);

	//Populating shared memory
	Colors[GTid.y][GTid.x] = gTexInput.Load(_Pixel);

	GroupMemoryBarrierWithGroupSync();

	float _MinVariance;

	float _MeanTemp;

	_MeanTemp = CalcVariance2(GTid.xy, _MinVariance);

	GroupMemoryBarrierWithGroupSync();

	Colors[GTid.y][GTid.x] = _MeanTemp;

	GroupMemoryBarrierWithGroupSync();

	if (GTid.x < SIZE || GTid.y < SIZE || GTid.x >= (ROW_SIZE + SIZE) || GTid.y >= (ROW_SIZE + SIZE) || _Pixel.x > gTextureSize.x || _Pixel.y > gTextureSize.y )
	{
		return;
	}

	uint _MinRow = GTid.y;
	uint _MinColumn = GTid.x;

	//Right Side

	uint _Row = GTid.y - SIZE;
	uint _Column = GTid.x - SIZE + SIZE;
	float _VarianceTemp = Variances[_Row][_Column];

	if (_VarianceTemp < _MinVariance)
	{
		_MinVariance = _VarianceTemp;
		_MinRow = GTid.y;
		_MinColumn = GTid.x + SIZE;
	}

	//Bottom Side

	_Row = GTid.y - SIZE + SIZE;
	_Column = GTid.x - SIZE;
	_VarianceTemp = Variances[_Row][_Column];

	if (_VarianceTemp < _MinVariance)
	{
		_MinVariance = _VarianceTemp;
		_MinRow = GTid.y + SIZE;
		_MinColumn = GTid.x;
	}

	//Right-Bottom Side

	_Row = GTid.y - SIZE + SIZE;
	_Column = GTid.x - SIZE + SIZE;
	_VarianceTemp = Variances[_Row][_Column];

	if (_VarianceTemp < _MinVariance)
	{
		_MinVariance = _VarianceTemp;
		_MinRow = GTid.y + SIZE;
		_MinColumn = GTid.x + SIZE;
	}

	gTexOutput[_Pixel.xy] = Colors[_MinRow][_MinColumn];

}